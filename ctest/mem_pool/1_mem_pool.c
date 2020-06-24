#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

struct mem_pool {
	uint8_t *mp_start_addr;
	uint8_t *mp_next_addr;
	uint32_t mp_obj_sz;
	uint32_t mp_obj_cnt;
	uint32_t mp_free_cnt;
	uint32_t mp_init_cnt;
};

void mem_pool_display(struct mem_pool *mp)
{
	int i;
	printf("%s\n", __func__);
	printf("---------------\n");
	printf("obj size - %u ", mp->mp_obj_sz);
	printf("obj cnt - %u ", mp->mp_obj_cnt);
	printf("start addr - %p ", mp->mp_start_addr);
	printf("next addr - %p ", mp->mp_next_addr);
	printf("free cnt - %u ", mp->mp_free_cnt);
	printf("init cnt - %u\n", mp->mp_init_cnt);
	for (i = 0; i < mp->mp_obj_cnt; i++) {
		printf("(%d - %u) ",
				i, *(uint32_t *)(mp->mp_start_addr + (i * mp->mp_obj_sz)));
	}
	printf("\n---------------\n");
	return;
}

struct mem_pool *mem_pool_create(uint32_t obj_size, uint32_t obj_cnt)
{
	struct mem_pool *mp;

	printf("%s\n", __func__);
	if (!(mp = malloc(sizeof(*mp))))
			goto done;
	
	if (!(mp->mp_start_addr = malloc(obj_size * obj_cnt))) {
		free(mp);
		mp = NULL;
		goto done;
	}
	mp->mp_next_addr = mp->mp_start_addr;
	mp->mp_obj_sz = obj_size;
	mp->mp_obj_cnt = obj_cnt;
	mp->mp_free_cnt = obj_cnt;
	mp->mp_init_cnt = 0;
done:
	mem_pool_display(mp);
	return mp;
}


void mem_pool_destroy(struct mem_pool *mp)
{
	printf("%s\n", __func__);
	free(mp->mp_start_addr);
	free(mp);
	return;
}

static inline void *mem_pool_inx_to_addr(struct mem_pool *mp, uint32_t inx)
{
	return (mp->mp_start_addr + (inx * mp->mp_obj_sz));
}


void *mem_pool_get_obj(struct mem_pool *mp)
{
	uint32_t *ptr;
	void *obj;

	printf("%s\n", __func__);
	if (mp->mp_init_cnt < mp->mp_obj_cnt) {
		ptr = mem_pool_inx_to_addr(mp, mp->mp_init_cnt); 
		*ptr = mp->mp_init_cnt + 1;
		mp->mp_init_cnt++;
	}
	obj = NULL;
	if (mp->mp_free_cnt > 0) {
		obj = mp->mp_next_addr;
		mp->mp_free_cnt--;
		if (mp->mp_free_cnt != 0) {
			mp->mp_next_addr = 
				mem_pool_inx_to_addr(mp, *(uint32_t *)mp->mp_next_addr);
		} else {
			mp->mp_next_addr = NULL;
			printf("mempool exhausted !!!\n");
		}
	}
	mem_pool_display(mp);
	return obj;
}

static inline uint32_t mem_pool_addr_to_inx(struct mem_pool *mp, void *addr)
{
	return (((uint32_t)((uint8_t *)addr - mp->mp_start_addr)) / mp->mp_obj_sz);
}

void mem_pool_put_obj(struct mem_pool *mp, void *obj)
{
	printf("%s\n", __func__);
	if (mp->mp_next_addr != NULL) {
		*(uint32_t *)obj = mem_pool_addr_to_inx(mp, mp->mp_next_addr);
	} else {
		*(uint32_t *)obj = mp->mp_obj_cnt;
	}
	mp->mp_next_addr =  obj;
	mp->mp_free_cnt++;
	mem_pool_display(mp);
	return;
}


#define OBJ_SIZE	8
#define OBJ_CNT		4

static inline void test_helper_get_all_objs(struct mem_pool *mp, char *ptr[])
{
	int i; 

	for (i = 0; i < mp->mp_obj_cnt; i++) {
		ptr[i] = mem_pool_get_obj(mp);
		snprintf(ptr[i], OBJ_SIZE, "object%d", i);
	}
	return;
}

static inline void test_helper_put_all_objs(struct mem_pool *mp, char *ptr[])
{
	int i; 

	for (i = 0; i < mp->mp_obj_cnt; i++) {
		mem_pool_put_obj(mp, ptr[i]);
	}
	return;
}


static inline void test_helper_print_all_objs(char *ptr[])
{
	int i;

	for (i = 0; i < OBJ_CNT; i++) {
		printf("%d - %s\n", i, ptr[i]);
	}
	return;
}

/*
 * Test case 1 -
 * keep getting objects till we exhaust the pool.
 * Then put all the objects back to the pool and finally get one object.
 * */
int test1()
{
	struct mem_pool *mp;
	char *ptr[OBJ_CNT];

	printf("Running Test 1...\n");

	mp = mem_pool_create(OBJ_SIZE, OBJ_CNT);
	
	test_helper_get_all_objs(mp, ptr);
	test_helper_print_all_objs(ptr);
	test_helper_put_all_objs(mp, ptr);

	ptr[0] = mem_pool_get_obj(mp);
	snprintf(ptr[0], OBJ_SIZE, "object1");
	printf("%s\n", ptr[0]);

	mem_pool_destroy(mp);

	printf("Finished Test 1\n");

	return 0;
}

/*
 * Test case 2 -
 * 1. keep getting objects till we exhaust the pool.
 * 2. Then put all the objects back to the pool
 * 3. Repeat 1 and 2 two times
 * */
int test2()
{
	struct mem_pool *mp;
	char *ptr[OBJ_CNT];
	int i;

	printf("Running Test 2...\n");

	mp = mem_pool_create(OBJ_SIZE, OBJ_CNT);

	for (i = 0; i < 3; i++) {
		test_helper_get_all_objs(mp, ptr);
		test_helper_print_all_objs(ptr);
		test_helper_put_all_objs(mp, ptr);
	}

	mem_pool_destroy(mp);

	printf("Finished Test 2\n");

	return 0;
}

/*
 * Test case 3 -
 * 1. Get 2 objects from the pool
 * 2. Put 1 object back to the pool
 * 3. Repeat 1 and 2
 * 4. Get 2 objects from the pool
 * 5. Get 1 more object from the pool. This will fail.
 * 6. Put 2 objects back to the pool.
 * 7. Get 1 object from the pool.
 * 8. Put 1 object back to the pool.
 * */
int test3()
{
	struct mem_pool *mp;
	char *ptr[OBJ_CNT];
	int i;

	printf("Running Test 3...\n");

	mp = mem_pool_create(OBJ_SIZE, OBJ_CNT);

	for (i = 0; i < 3; i++) {
		test_helper_get_all_objs(mp, ptr);
		test_helper_print_all_objs(ptr);
		test_helper_put_all_objs(mp, ptr);
	}

	mem_pool_destroy(mp);

	printf("Finished Test 3\n");

	return 0;
}

int main()
{
	(void) test1();
	(void) test2();
	return 0;
}
