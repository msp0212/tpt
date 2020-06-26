#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MP_OFFSET_INVALID 0xFFFFFFFF

#ifdef DEBUG
	#define DEBUG_PRINTF(format, ...) \
		printf("DEBUG:%s:%u:%s() "format"\n", \
				__FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
	#define DEBUG_PRINTF(format, ...) 
#endif

struct argo_mem_pool {
	uint32_t mp_obj_sz;
	uint32_t mp_obj_cnt;
	uint32_t mp_free_offset;
	struct argo_mem_pool **mp_keeper;
	void *mp_start_addr;
};

static inline void __mp_free_list_init(
								void *start_addr,
								uint32_t offset,
								uint32_t obj_sz,
								uint32_t obj_cnt)
{
	uint8_t *ptr;
	uint32_t i;

	ptr = start_addr;
	for (i = 0; i < obj_cnt - 1; i++) {
		DEBUG_PRINTF("ptr %p, offset %u", ptr, offset);
		offset += obj_sz;
		*(uint32_t *)ptr = offset;
		ptr += obj_sz;
	}
	DEBUG_PRINTF("ptr %p, offset %u", ptr, offset);
	*(uint32_t *)ptr = MP_OFFSET_INVALID;	

	return;
}

void argo_mem_pool_create(
						uint32_t obj_sz,
						uint32_t obj_cnt,
						struct argo_mem_pool **keeper)
{
	struct argo_mem_pool *mp;

	/*Objects should be aligned to 8-byte bounday*/
	obj_sz = ((obj_sz + 7) & (-8));
	*keeper = NULL;
	if(!(mp = malloc(sizeof(*mp) + (obj_sz * obj_cnt))))
		goto done;

	DEBUG_PRINTF("malloc'd address %p", mp);
	mp->mp_obj_sz = obj_sz;
	mp->mp_obj_cnt = obj_cnt;
	mp->mp_free_offset = 0;
	mp->mp_keeper = keeper;
	mp->mp_start_addr = (uint8_t *)mp + sizeof(*mp);
	DEBUG_PRINTF("start addr %p", mp->mp_start_addr);
	__mp_free_list_init(mp->mp_start_addr, 0, obj_sz, obj_cnt);
	*keeper = mp;	
done:
	return;
}

void argo_mem_pool_destroy(struct argo_mem_pool **keeper)
{
	free(*keeper);
	*keeper = NULL;
	return;
}

static void argo_mem_pool_expand(struct argo_mem_pool **mp)
{
	struct argo_mem_pool *tmp;
	uint32_t obj_cnt_old;
	uint32_t obj_cnt_new;

	DEBUG_PRINTF("Doubling the mem pool size");
	obj_cnt_old = (*mp)->mp_obj_cnt;
	obj_cnt_new = obj_cnt_old * 2;

	if (!(tmp = realloc(*mp,
					sizeof(*tmp) + ((*mp)->mp_obj_sz * obj_cnt_new)))) {
		goto done;
	}
	DEBUG_PRINTF("Realloc'd address %p", tmp);
	tmp->mp_obj_cnt = obj_cnt_new;
	tmp->mp_start_addr = (uint8_t *)tmp + sizeof(*tmp);
	tmp->mp_free_offset = (tmp->mp_obj_sz) * obj_cnt_old;
	__mp_free_list_init((uint8_t *)tmp->mp_start_addr + tmp->mp_free_offset,
							tmp->mp_free_offset, tmp->mp_obj_sz, obj_cnt_old);
	*(tmp->mp_keeper) = tmp;
	*mp = tmp;
done:
	return;
}

uint32_t argo_mem_pool_get(struct argo_mem_pool *mp)
{
	uint32_t offset;

	offset = mp->mp_free_offset;
	if (offset == MP_OFFSET_INVALID) {
		DEBUG_PRINTF("mem pool exhausted");
		argo_mem_pool_expand(&mp);
	}
	offset = mp->mp_free_offset;
	mp->mp_free_offset = *(uint32_t *)((uint8_t *)mp->mp_start_addr + offset);
	return offset;
}

void argo_mem_pool_put(struct argo_mem_pool *mp, uint32_t offset)
{
	*(uint32_t *)((uint8_t *)mp->mp_start_addr + offset) = mp->mp_free_offset;
	mp->mp_free_offset = offset;
	return;
}

void *argo_mem_pool_offset_to_addr(struct argo_mem_pool *mp, uint32_t offset)
{
	return (void *)((uint8_t *)mp->mp_start_addr + offset);
}

#define PASS "PASSED"
#define FAIL "FAILED"

#define OBJ_SZ 8
#define OBJ_CNT	 4

/*Get 1 objects from mem pool and put it back*/
int test1()
{
	uint32_t off;
	char *ptr;
	struct argo_mem_pool *mp;
	int ret = 0;

	argo_mem_pool_create(OBJ_SZ, OBJ_CNT, &mp);

	off = argo_mem_pool_get(mp);
	if (off == MP_OFFSET_INVALID) {
		DEBUG_PRINTF("offset returned from get call is invalid !\n");
	}
	ptr = argo_mem_pool_offset_to_addr(mp, off);
	strcpy(ptr, "ABC_001");
	printf("object contents: %s\n", ptr);
	argo_mem_pool_put(mp, off);
	argo_mem_pool_destroy(&mp);

	return ret;
}

/*Get 4 objects from mem pool and put all of them back*/
int test2()
{
	uint32_t off[OBJ_CNT];
	char *ptr;
	struct argo_mem_pool *mp;
	int ret = 0;
	int i;

	argo_mem_pool_create(OBJ_SZ, OBJ_CNT, &mp);

	for (i = 0; i < OBJ_CNT; i++) {
		off[i] = argo_mem_pool_get(mp);
		if (off[i] == MP_OFFSET_INVALID) {
			DEBUG_PRINTF("offset returned from get call is invalid !\n");
			ret = 1;
			goto done;
		}
		ptr = argo_mem_pool_offset_to_addr(mp, off[i]);
		sprintf(ptr, "ABC_%03d", i);
		printf("object contents: %s\n", ptr);
	}
	i--;
	for ( ; i >= 0; i--) {
		argo_mem_pool_put(mp, off[i]);
	}
	argo_mem_pool_destroy(&mp);
done:
	return ret;
}

/*Get 5 objects from mem pool and put all of them back*/
#define TEST_OBJ_CNT 5
int test3()
{
	uint32_t off[TEST_OBJ_CNT];
	char *ptr;
	struct argo_mem_pool *mp;
	int ret = 0;
	int i;

	argo_mem_pool_create(OBJ_SZ, OBJ_CNT, &mp);

	for (i = 0; i < TEST_OBJ_CNT; i++) {
		off[i] = argo_mem_pool_get(mp);
		if (off[i] == MP_OFFSET_INVALID) {
			DEBUG_PRINTF("offset returned from get call is invalid !\n");
			ret = 1;
			goto done;
		}
		ptr = argo_mem_pool_offset_to_addr(mp, off[i]);
		sprintf(ptr, "AB_%03d", i);
	}
	for (i = 0; i < TEST_OBJ_CNT; i++) {
		ptr = argo_mem_pool_offset_to_addr(mp, off[i]);
		printf("object contents: %s\n", ptr);
	}
	i--;
	for ( ; i >= 0; i--) {
		argo_mem_pool_put(mp, off[i]);
	}
	argo_mem_pool_destroy(&mp);
done:
	return ret;
}
#undef TEST_OBJ_CNT

/*Get 9 objects from mem pool and put all of them back*/
#define TEST_OBJ_CNT 9
int test4()
{
	uint32_t off[TEST_OBJ_CNT];
	char *ptr;
	struct argo_mem_pool *mp;
	int ret = 0;
	int i;

	argo_mem_pool_create(OBJ_SZ, OBJ_CNT, &mp);

	for (i = 0; i < TEST_OBJ_CNT; i++) {
		off[i] = argo_mem_pool_get(mp);
		if (off[i] == MP_OFFSET_INVALID) {
			DEBUG_PRINTF("offset returned from get call is invalid !\n");
			ret = 1;
			goto done;
		}
		ptr = argo_mem_pool_offset_to_addr(mp, off[i]);
		sprintf(ptr, "ABC_%03d", i);
	}
	for (i = 0; i < TEST_OBJ_CNT; i++) {
		ptr = argo_mem_pool_offset_to_addr(mp, off[i]);
		printf("object contents: %s\n", ptr);
	}
	i--;
	for ( ; i >= 0; i--) {
		argo_mem_pool_put(mp, off[i]);
	}
	argo_mem_pool_destroy(&mp);
done:
	return ret;
}
#undef TEST_OBJ_CNT

/*
 * 1. Get 4 objects from the pool
 * 2. Put 2 objects back to the pool
 * 3. Repeat step 1 and 2
 * 4. Get 5 more objects
 * 5. Put the remanining objects back to the pool
 * */
int test5()
{
	uint32_t off[16];
	char *ptr;
	struct argo_mem_pool *mp;
	int ret = 0;
	int i;

	argo_mem_pool_create(OBJ_SZ, OBJ_CNT, &mp);

	for (i = 0; i < 4; i++) {
		off[i] = argo_mem_pool_get(mp);
		if (off[i] == MP_OFFSET_INVALID) {
			DEBUG_PRINTF("offset returned from get call is invalid !\n");
			ret = 1;
			goto done;
		}
		ptr = argo_mem_pool_offset_to_addr(mp, off[i]);
		sprintf(ptr, "ABC_%03d", i);
	}
	for (i = 0; i < 4; i++) {
		ptr = argo_mem_pool_offset_to_addr(mp, off[i]);
		printf("object contents: %s\n", ptr);
	}
	argo_mem_pool_put(mp, off[0]);
	argo_mem_pool_put(mp, off[3]);

	off[0] = argo_mem_pool_get(mp);
	ptr = argo_mem_pool_offset_to_addr(mp, off[0]);
	sprintf(ptr, "ABC_%03d", 0);
	off[3] = argo_mem_pool_get(mp);
	ptr = argo_mem_pool_offset_to_addr(mp, off[3]);
	sprintf(ptr, "ABC_%03d", 3);
	off[4] = argo_mem_pool_get(mp);
	ptr = argo_mem_pool_offset_to_addr(mp, off[4]);
	sprintf(ptr, "ABC_%03d", 4);
	off[5] = argo_mem_pool_get(mp);
	ptr = argo_mem_pool_offset_to_addr(mp, off[5]);
	sprintf(ptr, "ABC_%03d", 5);
	for (i = 0; i < 6; i++) {
		ptr = argo_mem_pool_offset_to_addr(mp, off[i]);
		printf("object contents: %s\n", ptr);
	}
	argo_mem_pool_put(mp, off[4]);
	argo_mem_pool_put(mp, off[5]);

	off[6] = argo_mem_pool_get(mp);
	ptr = argo_mem_pool_offset_to_addr(mp, off[6]);
	sprintf(ptr, "ABC_%03d", 6);
	off[7] = argo_mem_pool_get(mp);
	ptr = argo_mem_pool_offset_to_addr(mp, off[7]);
	sprintf(ptr, "ABC_%03d", 7);
	off[8] = argo_mem_pool_get(mp);
	ptr = argo_mem_pool_offset_to_addr(mp, off[8]);
	sprintf(ptr, "ABC_%03d", 8);
	off[4] = argo_mem_pool_get(mp);
	ptr = argo_mem_pool_offset_to_addr(mp, off[4]);
	sprintf(ptr, "ABC_%03d", 4);
	off[5] = argo_mem_pool_get(mp);
	ptr = argo_mem_pool_offset_to_addr(mp, off[5]);
	sprintf(ptr, "ABC_%03d", 5);
	for (i = 0; i < 9; i++) {
		ptr = argo_mem_pool_offset_to_addr(mp, off[i]);
		printf("object contents: %s\n", ptr);
	}
	for (i = 0; i < 9; i++) {
		argo_mem_pool_put(mp, off[i]);
	}

	argo_mem_pool_destroy(&mp);
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
