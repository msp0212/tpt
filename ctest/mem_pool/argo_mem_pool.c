#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "argo_mem_pool.h"
#include "argo_log.h"
#include "argo_private.h"


/* argo collection queue object mem pool
 * 
 *
 *					   ---> mem pool obj
 *					  |
 *					 --- --- ---       --- 
 *	[0]		---->	|	|	|	| ... |	  |	 mem pool slab (upto 256MB)
 *					 --- --- ---       --- 
 *
 *  [1]		---->
 *  ..
 *  ..
 *  ..
 *  [4096]	---->
 *
 *
 * -Total memory addressable =  1024 * 256MB = 1024GB
 *
 * -Mem pool needs two inputs for initialization:
 *  1. mp_obj_sz - size of the fixed sized object this mem pool will hand out
 *  2. mp_obj_cnt - count of objects with which mem pool will get initialized
 *
 * -First 8 bytes(64 bits) in the each object is reserved for mem pool metadata.
 *  Yes, there is an overhead of 8 bytes per object
 * -28 lower significant bits(0-27) represent offset of object within a slab.
 *  Remaining higher significant bits(28-63) represent index of slab within pool
 *
 * -Each slab is contiguous memory block of size = mp_obj_sz * mp_obj_cnt
 *  Slab size is capped at 256MB (28 bits for offset)
 * -Each slab is logically divided into N equal sized objects
 *
 * -mem pool get() and put() operations run in O(1)
 * -addresses returned by get() operation are guaranteed to be 8-byte aligned
 *
 * -Implementation is done keeping in mind that this mem pool will be used
 *  for alloc/free of fixed size small objects. It should be used to reduce the
 *  alloc/free overhead for small sized objects.
 *
 * */



#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)


#define MP_METADATA_TYPE	uint64_t
#define MP_METADATA_SZ	(sizeof(MP_METADATA_TYPE))

#define MP_OFFSET_BITS		28
#define MP_OFFSET_MASK		(((MP_METADATA_TYPE)1 << MP_OFFSET_BITS) - 1)
#define MP_OFFSET_INVALID	MP_OFFSET_MASK

#define MP_SLAB_SIZE		(1 << MP_OFFSET_BITS)
#define MP_SLABS_MAX		4096	/*can support upto 1024GB*/


struct argo_mem_pool {
	void *slabs[MP_SLABS_MAX]; /*32KB*/
	MP_METADATA_TYPE  mp_free_next;
	uint64_t mp_slab_cnt;
	uint32_t mp_obj_sz;
	uint32_t mp_obj_cnt;
};


static inline void __mp_free_list_init(void *addr, uint32_t obj_sz,
										uint32_t obj_cnt, uint64_t slab_inx)
{
	uint64_t slab;
	MP_METADATA_TYPE next_free = 0;
	uint32_t i;

	slab = slab_inx << MP_OFFSET_BITS;
	for (i = 0; i < obj_cnt - 1; i++) {
		next_free += obj_sz;
		*(MP_METADATA_TYPE *)addr = slab | next_free;
		addr += obj_sz;
	}
	*(MP_METADATA_TYPE *)addr = slab | MP_OFFSET_INVALID;
}

static inline void __mp_adjust_size_and_count(uint32_t *sz, uint32_t *cnt)
{
	/*reserve space for mem pool metadata*/
	(*sz) += MP_METADATA_SZ;
	/*Align object on 8-byte boundary*/
	(*sz) = (((*sz) + 7) & (-8));
	if (unlikely(((*sz) * (*cnt)) > MP_SLAB_SIZE)) {
		_ARGO_LOG(AL_WARNING, "mem pool create with initial memory > %u",
															MP_SLAB_SIZE);
		*cnt = MP_SLAB_SIZE / (*sz);
	}
}

static inline int __mp_create_new_slab(struct argo_mem_pool *mp)
{	
	uint64_t slab_cnt = mp->mp_slab_cnt;
	uint32_t obj_sz = mp->mp_obj_sz;
	uint32_t obj_cnt = mp->mp_obj_cnt;

	if(unlikely(!(mp->slabs[slab_cnt] = ARGO_ALLOC(obj_sz * obj_cnt)))) {
		return 1;
	}
	__mp_free_list_init(mp->slabs[slab_cnt], obj_sz, obj_cnt, slab_cnt);
	mp->mp_free_next = (MP_METADATA_TYPE)slab_cnt << MP_OFFSET_BITS;
	(mp->mp_slab_cnt)++;
	return 0;
}

struct argo_mem_pool *argo_mem_pool_create(
						uint32_t obj_sz,
						uint32_t obj_cnt)
{
	struct argo_mem_pool *mp;

	__mp_adjust_size_and_count(&obj_sz, &obj_cnt);

	if(unlikely(!(mp = ARGO_ALLOC(sizeof(*mp)))))
		return NULL;

	memset(mp, 0, sizeof(*mp));
	mp->mp_obj_sz = obj_sz;
	mp->mp_obj_cnt = obj_cnt;

	if (__mp_create_new_slab(mp)) {
		ARGO_FREE(mp);
		return NULL;
	}

	return mp;
}

void argo_mem_pool_destroy(struct argo_mem_pool *mp)
{
	int i = 0;

	for (i = 0; i < MP_SLABS_MAX; i++) {
		if (mp->slabs[i]) {
			ARGO_FREE(mp->slabs[i]);
		}
	}
	ARGO_FREE(mp);
}

static int argo_mem_pool_expand(struct argo_mem_pool *mp)
{
	if (unlikely(mp->mp_slab_cnt == MP_SLABS_MAX)) {
		_ARGO_LOG(AL_ERROR, "All slabs exhausted. Can't allocate more memory");
		return 1;
	}
	if (__mp_create_new_slab(mp)) {
		return 2;
	}
	return 0;
}

#define __MP_METADATA_TO_INX_AND_OFFSET(metadata, inx, offset) \
	do { \
		(offset) = (metadata) & MP_OFFSET_MASK; \
		(inx) = (metadata) >> MP_OFFSET_BITS; \
	} while (0)

#define __MP_INX_AND_OFFSET_TO_ADDR(inx, offset) \
					(uint8_t *)((mp->slabs[(inx)]) + (offset))

void *argo_mem_pool_get(struct argo_mem_pool *mp)
{
	uint8_t *addr = NULL;
	uint64_t inx;
	uint32_t offset;

	__MP_METADATA_TO_INX_AND_OFFSET(mp->mp_free_next, inx, offset);

	if (unlikely(offset == MP_OFFSET_INVALID)) {
		/*current slab exhausted*/
		if (unlikely(argo_mem_pool_expand(mp)))
			return NULL;
		__MP_METADATA_TO_INX_AND_OFFSET(mp->mp_free_next, inx, offset);
	}

	addr = __MP_INX_AND_OFFSET_TO_ADDR(inx, offset);
	mp->mp_free_next = *(MP_METADATA_TYPE *)addr;
	/*remember slab and offset of the address before giving it out*/
	*(MP_METADATA_TYPE *)addr = (inx << MP_OFFSET_BITS) | offset;

	return addr + MP_METADATA_SZ;
}

void argo_mem_pool_put(struct argo_mem_pool *mp, void *addr)
{
	MP_METADATA_TYPE *metadata;
	MP_METADATA_TYPE new_free;

	metadata = (MP_METADATA_TYPE *)((uint8_t *)addr - MP_METADATA_SZ);

	new_free = *metadata;
	*metadata = mp->mp_free_next;
	mp->mp_free_next = new_free;
}
