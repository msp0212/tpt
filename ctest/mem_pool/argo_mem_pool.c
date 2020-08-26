#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "argo_mem_pool.h"
#if 0
#include "argo_log.h"
#include "argo_private.h"
#endif

#define ARGO_ALLOC malloc
#define ARGO_FREE free

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
 * -28 lesser significant bits(0-27) represent offset of object within a slab.
 *  Remaining more significant bits(28-63) represent index of slab within pool
 *
 * -Each slab is contiguous memory block of size = mp_obj_sz * mp_obj_cnt
 *  Slab size is capped at 256MB (28 bits for offset)
 * -Each slab is logically divided into N equal sized objects
 *
 * -mem pool get() and put() operations run in O(1)
 * -addresses returned by get() operation are guaranteed to be 8-byte aligned
 *
 * */



#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)


#define MP_METADATA_TYPE	uint64_t
#define MP_METADATA_SZ	(sizeof(MP_METADATA_TYPE))

#define MP_OFFSET_BITS		28
#define MP_OFFSET_INVALID	0x00FFFFFF

#define MP_SLAB_BITS		((MP_METADATA_SZ * 8) - MP_OFFSET_BITS)
#define MP_SLAB_MASK \
		((((MP_METADATA_TYPE)1 << MP_SLAB_BITS) - 1) << MP_OFFSET_BITS)
#define MP_SLAB_SIZE		(1 << MP_OFFSET_BITS)
#define MP_SLABS_MAX		4096	/*can support upto 1024GB*/


struct argo_mem_pool {
	void *slabs[MP_SLABS_MAX]; /*32KB*/
	MP_METADATA_TYPE  mp_free_next;
	uint32_t mp_obj_sz;
	uint32_t mp_obj_cnt;
};

static inline void __mp_free_list_init(
								void *start_addr,
								uint32_t obj_sz,
								uint32_t obj_cnt,
								MP_METADATA_TYPE slab_inx)
{
	uint8_t *ptr;
	uint32_t i;
	MP_METADATA_TYPE slab;
	MP_METADATA_TYPE next_offset = 0;

	ptr = start_addr;
	slab = slab_inx << MP_OFFSET_BITS;
	for (i = 0; i < obj_cnt - 1; i++) {
		next_offset += obj_sz;
		*(MP_METADATA_TYPE *)ptr = slab | next_offset;
		ptr += obj_sz;
	}
	*(MP_METADATA_TYPE *)ptr = slab | MP_OFFSET_INVALID;
	return;
}

struct argo_mem_pool *argo_mem_pool_create(
						uint32_t obj_sz,
						uint32_t obj_cnt)
{
	struct argo_mem_pool *mp;

	/*reserve space for mem pool metadata*/
	obj_sz += MP_METADATA_SZ;

	/*Align object on 8-byte boundary*/
	obj_sz = ((obj_sz + 7) & (-8));

	if (unlikely((obj_sz * obj_cnt) > MP_SLAB_SIZE)) {
		//_ARGO_LOG(AL_WARNING, "mem pool create with initial memory > 256MB !");
		DEBUG_PRINTF("obj_sz %u obj_cnt %u",obj_sz, obj_cnt);
		obj_cnt = MP_SLAB_SIZE / obj_sz;
		DEBUG_PRINTF("Reducing obj_cnt to %u", obj_cnt);
	}

	if(unlikely(!(mp = ARGO_ALLOC(sizeof(*mp)))))
		return NULL;

	memset(mp, 0, sizeof(*mp));
	if(unlikely(!(mp->slabs[0] = ARGO_ALLOC(obj_sz * obj_cnt)))) {
		ARGO_FREE(mp);
		return NULL;
	}

	mp->mp_obj_sz = obj_sz;
	mp->mp_obj_cnt = obj_cnt;
	mp->mp_free_next = 0;
	__mp_free_list_init(mp->slabs[0], obj_sz, obj_cnt, 0);
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
	return;
}

static int argo_mem_pool_expand(struct argo_mem_pool *mp, uint32_t inx)
{
	int ret = 0;

	DEBUG_PRINTF("Allocating one more slab of size %u at index %u\n",
							mp->mp_obj_sz * mp->mp_obj_cnt, inx);
	if (unlikely(inx == MP_SLABS_MAX)) {
		//_ARGO_LOG(AL_ERROR, "All slabs exhausted. Can't allocate more memory");
		ret = 1;
		goto done;
	}
	if(unlikely(!(mp->slabs[inx] =
					ARGO_ALLOC(mp->mp_obj_sz * mp->mp_obj_cnt)))) {
		ret = 1;
		goto done;
	}
	mp->mp_free_next = inx << MP_OFFSET_BITS;
	__mp_free_list_init(mp->slabs[inx],
							mp->mp_obj_sz, mp->mp_obj_cnt, inx);
done:
	return ret;
}

void *argo_mem_pool_get(struct argo_mem_pool *mp)
{
	uint8_t *addr = NULL;
	uint32_t offset;
	uint32_t inx;

	offset = mp->mp_free_next & ~MP_SLAB_MASK;
	inx = mp->mp_free_next >> MP_OFFSET_BITS;

	if (unlikely(offset == MP_OFFSET_INVALID)) {
		DEBUG_PRINTF("current mem slab exhausted. Trying to alloc one more");

		if (unlikely(argo_mem_pool_expand(mp, inx + 1)))
			return NULL;

		offset = mp->mp_free_next & (~MP_SLAB_MASK);
		inx = mp->mp_free_next >> MP_OFFSET_BITS;
	}

	addr = (uint8_t *)(mp->slabs[inx]) + offset;
	mp->mp_free_next = *(MP_METADATA_TYPE *)addr;
	addr += MP_METADATA_SZ;
	return addr;
}

void argo_mem_pool_put(struct argo_mem_pool *mp, void *addr)
{
	MP_METADATA_TYPE next_free;
	MP_METADATA_TYPE *metadata_ptr;

	metadata_ptr = (MP_METADATA_TYPE *)((uint8_t *)addr - MP_METADATA_SZ);

	next_free = *metadata_ptr;
	*metadata_ptr = mp->mp_free_next;
	mp->mp_free_next = next_free;
}
