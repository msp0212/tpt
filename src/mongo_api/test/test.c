#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <mongo_api.h>
#include <bson_api.h>

struct thread_data {
	bson_t *doc;
	mongo_context_pool_t *pool;
};

void test1();
void test2();
void test3();
void test4();
void test5();
void test6();
void *worker(void *data);

int main()
{
	test1();
	test3();
	test2();
	test3();
	test4();
	test5();
	test6();
	return 0;
}

void test1()
{
	mongo_context_pool_t *pool = NULL;
	int ret = 0;

	mongo_api_init();
	if ((ret = mongo_context_pool_init(&pool, "172.19.13.25", 4999, 
						50, 50, 1000, 2000)) < 0) {
		printf("Error [%d] [%s] in mongo_context_pool_init !\n",
						ret, mongo_err_msg(ret));
		printf("Test1 Failed\n");
		mongo_api_cleanup();
		goto exit;
	}
	if ((ret = mongo_context_pool_destroy(&pool)) < 0 ) {
		printf("Error [%d] [%s] in mongo_context_pool_init !\n", 
						ret, mongo_err_msg(ret));
		printf("Test1 Failed\n");
		mongo_api_cleanup();
	}
	mongo_api_cleanup();
	printf("Test1 Passed\n\n");
exit:
	return;
}


void test2()
{
	bson_element_t barr[2];
	arraydata_t arr;
	bson_edata_t arrdata[1] = {{0, }, };
	bson_t *bdoc = NULL;
	char *json = NULL;
	mongo_context_pool_t *pool = NULL;
	pthread_t tid[2];
	struct thread_data data;
	int i = 0;
	int ret = 0;

	mongo_api_init();
	
	arr.arrlen = 1;
	arr.arrdata = arrdata;

	arrdata[0].data.str = "this is 1st element";
	arrdata[0].type = BSON_STRING;

	barr[0].edata.type = BSON_STRING;
	barr[0].ename = "key_string";
	barr[0].edata.data.str = "Mohit Singh";

	barr[1].edata.type = BSON_ARRAY;
	barr[1].ename = "key_array";
	barr[1].edata.data.arr = &arr;


	if ((ret = bson_doc_create(barr, 2, &bdoc)) < 0) {
		printf("Error [%d] [%s] in bson_create_doc !\n",
				ret, bson_err_msg(ret));
		printf("Test2 FAILED !\n");
		goto exit;
	}
	if ((ret = bson_to_json(bdoc, &json, NULL )) < 0) {
		printf("Error [%d] [%s] in bson_to_json !\n",
				ret, bson_err_msg(ret));
		printf("Test2 FAILED !\n");
		bson_doc_destroy(bdoc);
		goto exit;
	}
	printf("json : %s\n", json);
	free(json); json = NULL;

	if ((ret = mongo_context_pool_init(&pool, "172.19.13.25", 4999, 
						1, 2, 1000, 2000)) < 0) {
		printf("Error [%d] [%s] in mongo_context_pool_init !\n",
						ret, mongo_err_msg(ret));
		printf("Test2 Failed\n");
		bson_doc_destroy(bdoc);
		mongo_api_cleanup();
		goto exit;
	}
	data.pool = pool;
	data.doc = bdoc;
	for (i = 0; i < 2; i++) {
		pthread_create(&tid[i], NULL, worker, (void *)&data);
	}
	for (i = 0; i < 2l; i++) {
		pthread_join(tid[i], NULL);
	}
	if ((ret = mongo_context_pool_destroy(&pool)) < 0 ) {
		printf("Error [%d] [%s] in mongo_context_pool_init !\n", 
						ret, mongo_err_msg(ret));
		printf("Test2 Failed\n");
		bson_doc_destroy(bdoc);
		mongo_api_cleanup();
	}
	bson_doc_destroy(bdoc);
	mongo_api_cleanup();
	printf("Test2 Passed\n\n");
exit:
	return;
}

void *worker(void *data)
{
	struct thread_data *d;
	int ret = 0;

	d = data;

	if ((ret = mongo_insert_doc(d->pool, "test", "test", d->doc)) < 0) {
		printf("Error [%d] [%s] in mongo_insert_doc !\n",
						ret, mongo_err_msg(ret));
	}
	printf("Work Done !!\n");
	return NULL;

}

void test3()
{
	bson_element_t barr[1];
	bson_t *bdoc = NULL;
	char *json = NULL;
	mongo_context_pool_t *pool = NULL;
	int ret = 0;
	bool exists = false;

	mongo_api_init();
	barr[0].edata.type = BSON_STRING;
	barr[0].ename = "key_string";
	barr[0].edata.data.str = "Mohit Singh";

	if ((ret = bson_doc_create(barr, 1, &bdoc)) < 0) {
		printf("Error [%d] [%s] in bson_create_doc !\n",
				ret, bson_err_msg(ret));
		printf("Test3 FAILED !\n");
		mongo_api_cleanup();
		goto exit;
	}
	if ((ret = bson_to_json(bdoc, &json, NULL )) < 0) {
		printf("Error [%d] [%s] in bson_to_json !\n",
				ret, bson_err_msg(ret));
		printf("Test3 FAILED !\n");
		mongo_api_cleanup();
		bson_doc_destroy(bdoc);
		goto exit;
	}
	printf("json : %s\n", json);
	free(json); json = NULL;

	if ((ret = mongo_context_pool_init(&pool, "172.19.13.25", 4999, 
						1, 2, 1000, 2000)) < 0) {
		printf("Error [%d] [%s] in mongo_context_pool_init !\n",
						ret, mongo_err_msg(ret));
		printf("Test3 Failed\n");
		bson_doc_destroy(bdoc);
		mongo_api_cleanup();
		goto exit;
	}
	if ((ret = mongo_doc_exists(pool, "test", "test", bdoc, &exists)) < 0) {
		printf("Error [%d] [%s] in mongo_doc_exists !\n",
	                                                ret, mongo_err_msg(ret));
		printf("Test3 Failed\n");
		bson_doc_destroy(bdoc);
		mongo_api_cleanup();
		goto exit;
	}
	if (exists == true) {
		printf("DOC EXISTS!\n");
	} else {
		printf("DOC DOES NOT EXIST!\n");
	}
	if ((ret = mongo_context_pool_destroy(&pool)) < 0 ) {
		printf("Error [%d] [%s] in mongo_context_pool_init !\n", 
						ret, mongo_err_msg(ret));
		printf("Test3 Failed\n");
		bson_doc_destroy(bdoc);
		mongo_api_cleanup();
	}
	bson_doc_destroy(bdoc);
	mongo_api_cleanup();
	printf("Test3 Passed\n\n");
exit:
	return;
}

void test4()
{
	bson_element_t barr[2];
	bson_t *query = NULL;
	bson_t *tmp = NULL;
	bson_t *update = NULL;
	char *json = NULL;
	mongo_context_pool_t *pool = NULL;
	int ret = 0;

	mongo_api_init();
	barr[0].edata.type = BSON_STRING;
	barr[0].ename = "key_string";
	barr[0].edata.data.str = "Mohit Singh";

	if ((ret = bson_doc_create(barr, 1, &query)) < 0) {
		printf("Error [%d] [%s] in bson_create_doc !\n",
				ret, bson_err_msg(ret));
		printf("Test4 FAILED !\n");
		mongo_api_cleanup();
		goto exit;
	}
	if ((ret = bson_to_json(query, &json, NULL )) < 0) {
		printf("Error [%d] [%s] in bson_to_json !\n",
				ret, bson_err_msg(ret));
		printf("Test4 FAILED !\n");
		mongo_api_cleanup();
		bson_doc_destroy(query);
		goto exit;
	}
	printf("query json : %s\n", json);
	free(json); json = NULL;
	
	barr[0].edata.type = BSON_STRING;
	barr[0].ename = "key_array";
	barr[0].edata.data.str = "This is 2nd Element";

	if ((ret = bson_doc_create(barr, 1, &tmp)) < 0) {
		printf("Error [%d] [%s] in bson_create_doc !\n",
				ret, bson_err_msg(ret));
		printf("Test4 FAILED !\n");
		bson_doc_destroy(query);
		mongo_api_cleanup();
		goto exit;
	}
	if ((ret = bson_to_json(tmp, &json, NULL )) < 0) {
		printf("Error [%d] [%s] in bson_to_json !\n",
				ret, bson_err_msg(ret));
		printf("Test4 FAILED !\n");
		mongo_api_cleanup();
		bson_doc_destroy(query);
		bson_doc_destroy(tmp);
		goto exit;
	}
	printf("tmp json : %s\n", json);
	free(json); json = NULL;
	
	barr[0].edata.type = BSON_DOC;
	barr[0].ename = "$push";
	barr[0].edata.data.doc = tmp;

	if ((ret = bson_doc_create(barr, 1, &update)) < 0) {
		printf("Error [%d] [%s] in bson_create_doc !\n",
				ret, bson_err_msg(ret));
		printf("Test4 FAILED !\n");
		bson_doc_destroy(query);
		bson_doc_destroy(tmp);
		mongo_api_cleanup();
		goto exit;
	}
	if ((ret = bson_to_json(update, &json, NULL )) < 0) {
		printf("Error [%d] [%s] in bson_to_json !\n",
				ret, bson_err_msg(ret));
		printf("Test4 FAILED !\n");
		mongo_api_cleanup();
		bson_doc_destroy(query);
		bson_doc_destroy(tmp);
		bson_doc_destroy(update);
		goto exit;
	}
	printf("update json : %s\n", json);
	free(json); json = NULL;

	if ((ret = mongo_context_pool_init(&pool, "172.19.13.25", 4999, 
						1, 2, 1000, 2000)) < 0) {
		printf("Error [%d] [%s] in mongo_context_pool_init !\n",
						ret, mongo_err_msg(ret));
		printf("Test4 Failed\n");
		bson_doc_destroy(query);
		bson_doc_destroy(tmp);
		bson_doc_destroy(update);
		mongo_api_cleanup();
		goto exit;
	}
	if ((ret = mongo_doc_update(pool, "test", "test", query, update)) < 0) {
		printf("Error [%d] [%s] in mongo_doc_exists !\n",
	                                                ret, mongo_err_msg(ret));
		printf("Test4 Failed\n");
		bson_doc_destroy(query);
		bson_doc_destroy(tmp);
		bson_doc_destroy(update);
		mongo_api_cleanup();
		goto exit;
	}
	if ((ret = mongo_context_pool_destroy(&pool)) < 0 ) {
		printf("Error [%d] [%s] in mongo_context_pool_init !\n", 
						ret, mongo_err_msg(ret));
		printf("Test4 Failed\n");
		bson_doc_destroy(query);
		bson_doc_destroy(tmp);
		bson_doc_destroy(update);
		mongo_api_cleanup();
	}
	bson_doc_destroy(query);
	bson_doc_destroy(tmp);
	bson_doc_destroy(update);
	mongo_api_cleanup();
	printf("Test4 Passed\n\n");
exit:
	return;
}

void test5()
{
	bson_element_t barr[2];
	bson_t *keys = NULL;
	bson_t *bdoc = NULL;
	char *json = NULL;
	mongo_context_pool_t *pool = NULL;
	int ret = 0;
	long long utc_ms;
	struct timeval tv;

	gettimeofday(&tv, NULL);

	utc_ms = (unsigned long long)(tv.tv_sec) * 1000 + 
			(unsigned long long)(tv.tv_usec) / 1000;
	mongo_api_init();

	barr[0].edata.type = BSON_DATETIME;
	barr[0].ename = "createdAt";
	barr[0].edata.data.utc_ms = utc_ms;
	
	barr[1].edata.type = BSON_STRING;
	barr[1].ename = "key_string";
	barr[1].edata.data.str = "Mohit Singh";

	if ((ret = bson_doc_create(barr, 2, &bdoc)) < 0) {
		printf("Error [%d] [%s] in bson_create_doc !\n",
				ret, bson_err_msg(ret));
		printf("Test5 FAILED !\n");
		mongo_api_cleanup();
		goto exit;
	}
	if ((ret = bson_to_json(bdoc, &json, NULL )) < 0) {
		printf("Error [%d] [%s] in bson_to_json !\n",
				ret, bson_err_msg(ret));
		printf("Test5 FAILED !\n");
		mongo_api_cleanup();
		bson_doc_destroy(bdoc);
		goto exit;
	}
	printf("doc json : %s\n", json);
	free(json); json = NULL;
	
	barr[0].edata.type = BSON_INT32;
	barr[0].ename = "createdAt";
	barr[0].edata.data.i32 = 1;

	if ((ret = bson_doc_create(barr, 1, &keys)) < 0) {
		printf("Error [%d] [%s] in bson_create_doc !\n",
				ret, bson_err_msg(ret));
		printf("Test5 FAILED !\n");
		bson_doc_destroy(bdoc);
		mongo_api_cleanup();
		goto exit;
	}
	if ((ret = bson_to_json(keys, &json, NULL )) < 0) {
		printf("Error [%d] [%s] in bson_to_json !\n",
				ret, bson_err_msg(ret));
		printf("Test5 FAILED !\n");
		mongo_api_cleanup();
		bson_doc_destroy(bdoc);
		bson_doc_destroy(keys);
		goto exit;
	}
	printf("keys json : %s\n", json);
	free(json); json = NULL;
	
	if ((ret = mongo_context_pool_init(&pool, "172.19.13.25", 4999, 
						1, 2, 1000, 2000)) < 0) {
		printf("Error [%d] [%s] in mongo_context_pool_init !\n",
						ret, mongo_err_msg(ret));
		printf("Test5 Failed\n");
		bson_doc_destroy(bdoc);
		bson_doc_destroy(keys);
		mongo_api_cleanup();
		goto exit;
	}
	if ((ret = mongo_coll_create_ttl_index(pool, "test", "test1", keys, 10)) < 0) {
		printf("Error [%d] [%s] in mongo_coll_create_ttl_index !\n",
	                                                ret, mongo_err_msg(ret));
		printf("Test5 Failed\n");
		bson_doc_destroy(bdoc);
		bson_doc_destroy(keys);
		mongo_api_cleanup();
		goto exit;
	}
	if ((ret = mongo_insert_doc(pool, "test", "test1", bdoc)) < 0) {
		printf("Error [%d] [%s] in mongo_insert_doc !\n",
						ret, mongo_err_msg(ret));
		bson_doc_destroy(bdoc);
		bson_doc_destroy(keys);
		mongo_api_cleanup();
		goto exit;
	}
	if ((ret = mongo_context_pool_destroy(&pool)) < 0 ) {
		printf("Error [%d] [%s] in mongo_context_pool_init !\n", 
						ret, mongo_err_msg(ret));
		printf("Test5 Failed\n");
		bson_doc_destroy(bdoc);
		bson_doc_destroy(keys);
		mongo_api_cleanup();
	}
		bson_doc_destroy(bdoc);
		bson_doc_destroy(keys);
	mongo_api_cleanup();
	printf("Test5 Passed\n\n");
exit:
	return;
}

void test6()
{
	bson_element_t barr[1];
	bson_t *bdoc = NULL;
	char *json = NULL;
	mongo_context_pool_t *pool = NULL;
	int ret = 0;

	mongo_api_init();
	barr[0].edata.type = BSON_STRING;
	barr[0].ename = "key_string";
	barr[0].edata.data.str = "Mohit Singh";

	if ((ret = bson_doc_create(barr, 1, &bdoc)) < 0) {
		printf("Error [%d] [%s] in bson_create_doc !\n",
				ret, bson_err_msg(ret));
		printf("Test6 FAILED !\n");
		mongo_api_cleanup();
		goto exit;
	}
	if ((ret = bson_to_json(bdoc, &json, NULL )) < 0) {
		printf("Error [%d] [%s] in bson_to_json !\n",
				ret, bson_err_msg(ret));
		printf("Test6 FAILED !\n");
		mongo_api_cleanup();
		bson_doc_destroy(bdoc);
		goto exit;
	}
	printf("json : %s\n", json);
	free(json); json = NULL;

	if ((ret = mongo_context_pool_init(&pool, "172.19.13.25", 4999, 
						1, 2, 1000, 2000)) < 0) {
		printf("Error [%d] [%s] in mongo_context_pool_init !\n",
						ret, mongo_err_msg(ret));
		printf("Test3 Failed\n");
		bson_doc_destroy(bdoc);
		mongo_api_cleanup();
		goto exit;
	}
	if ((ret = mongo_doc_remove(pool, "test", "test", bdoc)) < 0) {
		printf("Error [%d] [%s] in mongo_doc_remove !\n",
	                                                ret, mongo_err_msg(ret));
		printf("Test6 Failed\n");
		bson_doc_destroy(bdoc);
		mongo_api_cleanup();
		goto exit;
	}
	if ((ret = mongo_context_pool_destroy(&pool)) < 0 ) {
		printf("Error [%d] [%s] in mongo_context_pool_init !\n", 
						ret, mongo_err_msg(ret));
		printf("Test6 Failed\n");
		bson_doc_destroy(bdoc);
		mongo_api_cleanup();
	}
	bson_doc_destroy(bdoc);
	mongo_api_cleanup();
	printf("Test6 Passed\n\n");
exit:
	return;
}
