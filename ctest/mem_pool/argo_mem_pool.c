#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MP_OFFSET_INVALID 0x00FFFFFF

#ifdef DEBUG
	#define DEBUG_PRINTF(format, ...) \
		printf("DEBUG:%s:%u:%s() "format"\n", \
				__FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
	#define DEBUG_PRINTF(format, ...) 
#endif

#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)

#define MP_METADATA_SZ	(sizeof(uint32_t))
#define MP_SLAB_BITS	4
#define MP_OFFSET_BITS	((MP_METADATA_SZ * 8) - MP_SLAB_BITS)
#define	MP_SLAB_SIZE	(1 << MP_OFFSET_BITS)
#define MP_SLAB_CNT		(1 << MP_SLAB_BITS)
#define MP_SLAB_MASK_32	((MP_SLAB_CNT - 1) << MP_OFFSET_BITS)

struct argo_mem_pool {
	void *mp_start_addr[MP_SLAB_CNT];
	uint32_t mp_obj_sz;
	uint32_t mp_obj_cnt;
	uint32_t mp_free_offset;
};

static inline void __mp_free_list_init(
								void *start_addr,
								uint32_t obj_sz,
								uint32_t obj_cnt,
								uint32_t slab_inx)
{
	uint8_t *ptr;
	uint32_t i;
	uint32_t slab_bits;
	uint32_t offset = 0;

	ptr = start_addr;
	slab_bits = slab_inx << MP_OFFSET_BITS;
	for (i = 0; i < obj_cnt - 1; i++) {
		DEBUG_PRINTF("ptr %p, offset %u", ptr, offset);
		offset += obj_sz;
		*(uint32_t *)ptr = slab_bits | offset;
		ptr += obj_sz;
	}
	DEBUG_PRINTF("ptr %p, offset %u", ptr, offset);
	*(uint32_t *)ptr = slab_bits | MP_OFFSET_INVALID;
	return;
}

struct argo_mem_pool *argo_mem_pool_create(
						uint32_t obj_sz,
						uint32_t obj_cnt)
{
	struct argo_mem_pool *mp;
	/*reserve first 4 bytes for storing mem pool metadata*/
	obj_sz += MP_METADATA_SZ;
	/*Align object on 8-byte boundary*/
	obj_sz = ((obj_sz + 7) & (-8));
	if (unlikely((obj_sz * obj_cnt) > MP_SLAB_SIZE)) {
		DEBUG_PRINTF("Requesting more than 256MB");
		DEBUG_PRINTF("obj_sz %u obj_cnt %u",obj_sz, obj_cnt);
		obj_cnt = MP_SLAB_SIZE / obj_sz;
		DEBUG_PRINTF("Reducing obj_cnt to %u", obj_cnt);
	}
	if(unlikely(!(mp = malloc(sizeof(*mp)))))
		goto done;
	memset(mp, 0, sizeof(*mp));
	if(unlikely(!(mp->mp_start_addr[0] = malloc(obj_sz * obj_cnt)))) {
		DEBUG_PRINTF("malloc failed");
		free(mp);
		mp = NULL;
		goto done;
	}
	mp->mp_obj_sz = obj_sz;
	mp->mp_obj_cnt = obj_cnt;
	mp->mp_free_offset = 0;
	__mp_free_list_init(mp->mp_start_addr[0], obj_sz, obj_cnt, 0);
done:
	return mp;
}

void argo_mem_pool_destroy(struct argo_mem_pool *mp)
{
	int i = 0;
	for (i = 0; i < MP_SLAB_CNT; i++) {
		if (mp->mp_start_addr[i]) {
			free(mp->mp_start_addr[i]);
		}
	}
	free(mp);
	return;
}

static int argo_mem_pool_expand(struct argo_mem_pool *mp, uint32_t inx)
{
	int ret = 0;

	DEBUG_PRINTF("Allocating one more slab of size %u at index %u\n",
							mp->mp_obj_sz * mp->mp_obj_cnt, inx);
	if (unlikely(inx == MP_SLAB_CNT)) {
		DEBUG_PRINTF("All slabs exhausted. Can't allocate more memory");
		ret = 1;
		goto done;
	}
	if(unlikely(!(mp->mp_start_addr[inx] =
					malloc(mp->mp_obj_sz * mp->mp_obj_cnt)))) {
		DEBUG_PRINTF("malloc failed");
		ret = 1;
		goto done;
	}
	mp->mp_free_offset = inx << MP_OFFSET_BITS;
	__mp_free_list_init(mp->mp_start_addr[inx],
							mp->mp_obj_sz, mp->mp_obj_cnt, inx);
	DEBUG_PRINTF("mp_free_offset %x\n", mp->mp_free_offset);
done:
	return ret;
}

void *argo_mem_pool_get(struct argo_mem_pool *mp)
{
	uint8_t *addr = NULL;
	uint32_t off;
	uint32_t inx;

	DEBUG_PRINTF("mp_free_offset %x\n", mp->mp_free_offset);
	off = mp->mp_free_offset & (~MP_SLAB_MASK_32);
	inx = mp->mp_free_offset >> MP_OFFSET_BITS;
	if (off == MP_OFFSET_INVALID) {
		DEBUG_PRINTF("current mem slab exhausted. Trying to alloc one more");
		if (argo_mem_pool_expand(mp, inx + 1))
			goto done;
		off = mp->mp_free_offset & (~MP_SLAB_MASK_32);
		inx = mp->mp_free_offset >> MP_OFFSET_BITS;
	}
	addr = (uint8_t *)(mp->mp_start_addr[inx]) + off;
	mp->mp_free_offset = *(uint32_t *)addr;
	DEBUG_PRINTF("mp_free_offset %x\n", mp->mp_free_offset);
	addr += MP_METADATA_SZ;
done:
	return addr;
}

void argo_mem_pool_put(struct argo_mem_pool *mp, void *addr)
{
	uint32_t offset;

	offset = *(uint32_t *)((uint8_t *)addr - MP_METADATA_SZ);
	*(uint32_t *)((uint8_t *)addr - MP_METADATA_SZ) = mp->mp_free_offset;
	mp->mp_free_offset = offset;

	return;
}

#define PASS "PASSED"
#define FAIL "FAILED"

#define OBJ_SZ 8
#define OBJ_CNT	 4

/*Get 1 objects from mem pool and put it back*/
int test1()
{
	char *ptr;
	struct argo_mem_pool *mp;
	int ret = 0;

	mp = argo_mem_pool_create(OBJ_SZ, OBJ_CNT);

	ptr = argo_mem_pool_get(mp);
	if (!ptr) {
		DEBUG_PRINTF("argo_mem_pool_get returned NULL");
	}
	strcpy(ptr, "ABC_001");
	printf("object contents: %s\n", ptr);
	argo_mem_pool_put(mp, ptr);
	argo_mem_pool_destroy(mp);

	return ret;
}

/*Get 4 objects from mem pool and put all of them back*/
int test2()
{
	char *ptr[OBJ_CNT];
	struct argo_mem_pool *mp;
	int ret = 0;
	int i;

	mp = argo_mem_pool_create(OBJ_SZ, OBJ_CNT);

	for (i = 0; i < OBJ_CNT; i++) {
		ptr[i] = argo_mem_pool_get(mp);
		if (!ptr[i]) {
			DEBUG_PRINTF("argo_mem_pool_get returned NULL");
			ret = 1;
			goto done;
		}
		sprintf(ptr[i], "ABC_%03d", i);
		printf("object contents: %s\n", ptr[i]);
	}
	i--;
	for ( ; i >= 0; i--) {
		argo_mem_pool_put(mp, ptr[i]);
	}
	argo_mem_pool_destroy(mp);
done:
	return ret;
}

/*Get 5 objects from mem pool and put all of them back*/
#define TEST_OBJ_CNT 5
int test3()
{
	char *ptr[TEST_OBJ_CNT];
	struct argo_mem_pool *mp;
	int ret = 0;
	int i;

	mp = argo_mem_pool_create(OBJ_SZ, OBJ_CNT);

	for (i = 0; i < TEST_OBJ_CNT; i++) {
		ptr[i] = argo_mem_pool_get(mp);
		if (!ptr[i]) {
			DEBUG_PRINTF("offset returned from get call is invalid !\n");
			ret = 1;
			goto done;
		}
		sprintf(ptr[i], "ABC_%03d", i);
	}
	for (i = 0; i < TEST_OBJ_CNT; i++) {
		printf("object contents: %s\n", ptr[i]);
	}
	i--;
	for ( ; i >= 0; i--) {
		argo_mem_pool_put(mp, ptr[i]);
	}
	argo_mem_pool_destroy(mp);
done:
	return ret;
}
#undef TEST_OBJ_CNT

/*Get 9 objects from mem pool and put all of them back*/
#define TEST_OBJ_CNT 9
int test4()
{
	char *ptr[TEST_OBJ_CNT];
	struct argo_mem_pool *mp;
	int ret = 0;
	int i;

	mp = argo_mem_pool_create(OBJ_SZ, OBJ_CNT);

	for (i = 0; i < TEST_OBJ_CNT; i++) {
		ptr[i] = argo_mem_pool_get(mp);
		if (!ptr[i]) {
			DEBUG_PRINTF("argo_mem_pool_get returned NULL");
			ret = 1;
			goto done;
		}
		sprintf(ptr[i], "ABC_%03d", i);
	}
	for (i = 0; i < TEST_OBJ_CNT; i++) {
		printf("object contents: %s\n", ptr[i]);
	}
	i--;
	for ( ; i >= 0; i--) {
		argo_mem_pool_put(mp, ptr[i]);
	}
	argo_mem_pool_destroy(mp);
done:
	return ret;
}
#undef TEST_OBJ_CNT

int test5()
{
	char *ptr[16];
	struct argo_mem_pool *mp;
	int ret = 0;
	int i;

	mp = argo_mem_pool_create(OBJ_SZ, OBJ_CNT);

	for (i = 0; i < 4; i++) {
		ptr[i] = argo_mem_pool_get(mp);
		if (!ptr[i]) {
			DEBUG_PRINTF("argo_mem_pool_get returned NULL");
			ret = 1;
			goto done;
		}
		sprintf(ptr[i], "ABC_%03d", i);
	}
	for (i = 0; i < 4; i++) {
		printf("object contents: %s\n", ptr[i]);
	}
	argo_mem_pool_put(mp, ptr[0]);
	argo_mem_pool_put(mp, ptr[3]);

	ptr[0] = argo_mem_pool_get(mp);
	sprintf(ptr[0], "ABC_%03d", 0);
	ptr[3] = argo_mem_pool_get(mp);
	sprintf(ptr[3], "ABC_%03d", 3);
	ptr[4] = argo_mem_pool_get(mp);
	sprintf(ptr[4], "ABC_%03d", 4);
	ptr[5] = argo_mem_pool_get(mp);
	sprintf(ptr[5], "ABC_%03d", 5);
	for (i = 0; i < 6; i++) {
		printf("object contents: %s\n", ptr[i]);
	}
	argo_mem_pool_put(mp, ptr[4]);
	argo_mem_pool_put(mp, ptr[5]);

	ptr[6] = argo_mem_pool_get(mp);
	sprintf(ptr[6], "ABC_%03d", 6);
	ptr[7] = argo_mem_pool_get(mp);
	sprintf(ptr[7], "ABC_%03d", 7);
	ptr[8] = argo_mem_pool_get(mp);
	sprintf(ptr[8], "ABC_%03d", 8);
	ptr[4] = argo_mem_pool_get(mp);
	sprintf(ptr[4], "ABC_%03d", 4);
	ptr[5] = argo_mem_pool_get(mp);
	sprintf(ptr[5], "ABC_%03d", 5);
	for (i = 0; i < 9; i++) {
		printf("object contents: %s\n", ptr[i]);
	}
	for (i = 0; i < 9; i++) {
		argo_mem_pool_put(mp, ptr[i]);
	}
	argo_mem_pool_destroy(mp);
done:
	return ret;
}
#undef TEST_OBJ_CNT


int main()
{
	printf("test1 - %s\n", test1() ? FAIL : PASS);
	printf("test2 - %s\n", test2() ? FAIL : PASS);
	printf("test3 - %s\n", test3() ? FAIL : PASS);
	printf("test4 - %s\n", test4() ? FAIL : PASS);
	printf("test5 - %s\n", test5() ? FAIL : PASS);
	return 0;
}
