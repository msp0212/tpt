#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "argo_mem_pool.h"

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

int test6(void) 
{
	struct argo_mem_pool *mp;
	void *addr;
	int i;

	mp = argo_mem_pool_create(8, 2);
	for (i = 0; i < 50; i++) {
		addr = argo_mem_pool_get(mp);
	}
	addr++;
	return 0;
}


int main()
{
	printf("test1 - %s\n", test1() ? FAIL : PASS);
	printf("test2 - %s\n", test2() ? FAIL : PASS);
	printf("test3 - %s\n", test3() ? FAIL : PASS);
	printf("test4 - %s\n", test4() ? FAIL : PASS);
	printf("test5 - %s\n", test5() ? FAIL : PASS);
	printf("test6 - %s\n", test6() ? FAIL : PASS);
	return 0;
}
