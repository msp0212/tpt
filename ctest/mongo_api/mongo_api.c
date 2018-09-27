/*System Headers*/
#include <stdio.h>
#include <mongoc.h>
#include <bson.h>
/*Local Headers*/
#include <mongo_api.h>
#include <mongo_errors.h>

#define DEFAULT_MIN_SIZE 1
#define DEFAULT_MAX_SIZE 10
#define DEFAULT_CONN_TIMEOUT 5000 /*5000ms = 5s*/
#define DEFAULT_OP_TIMEOUT 5000 /*5000ms = 5s*/

struct mongo_context_pool {
	mongoc_client_pool_t *pool;
};

static int mongo_check_pool_params(int *min_size, int *max_size, 
					int *conn_timeout, int *op_timout);

void mongo_api_init()
{
	mongoc_init();
	return;
}

void mongo_api_cleanup()
{
	mongoc_cleanup();
	return;
}

int mongo_context_pool_init(mongo_context_pool_t **mpool, char *ip, int port,
					int min_size, int max_size, 
					int conn_timeout, int op_timeout)
{
	int ret = 0;
	char uri[2048] = {0,};
	mongoc_uri_t *mongo_uri = NULL;
	mongoc_client_pool_t *pool = NULL;

	if (mpool == NULL || ip == NULL) {
		ret = MONGO_ERR_NULL_PARAMS;
		goto exit;
	}
	if (mongo_check_pool_params(&min_size, &max_size, &conn_timeout,
							&op_timeout) < 0) {
		ret = MONGO_ERR_POOL_PARAMS;
		goto exit;
	}
	snprintf(uri, sizeof(uri), "mongodb://%s:%d/?minPoolSize=%d"
				"&maxPoolSize=%d&connectTimeoutMS=%d"
				"socketTimeoutMS=%d", ip, port, min_size,
					max_size, conn_timeout, op_timeout);
	if((mongo_uri = mongoc_uri_new(uri)) == NULL) {
		ret = MONGO_ERR_URI_NEW;
		goto exit;
	}
	if ((pool = mongoc_client_pool_new(mongo_uri)) == NULL ) {
		ret = MONGO_ERR_POOL_NEW;
		mongoc_uri_destroy(mongo_uri);
		goto exit;
	}
	if ((*mpool = malloc(sizeof(*mpool))) == NULL) {
		ret = MONGO_ERR_MEM_ALLOC;
		mongoc_uri_destroy(mongo_uri);
		mongoc_client_pool_destroy(pool);
		goto exit;
	}
	(*mpool)->pool = pool;
	mongoc_uri_destroy(mongo_uri);
exit:
	return ret;
}

int mongo_context_pool_destroy(mongo_context_pool_t **mpool)
{
	int ret = 0;
	
	if (mpool == NULL) {
		ret = MONGO_ERR_NULL_PARAMS;
		goto exit;
	}
	mongoc_client_pool_destroy((*mpool)->pool);
	free(*mpool); *mpool = NULL;
exit:
	return ret;
}

int mongo_insert_doc(mongo_context_pool_t *mpool, char *db_name, 
						char *coll_name, bson_t *doc)
{
	int ret = 0;
	mongoc_client_t *client = NULL;
	mongoc_collection_t *coll = NULL;

	if (mpool == NULL || db_name == NULL || coll_name == NULL 
							|| doc == NULL) {
		ret = MONGO_ERR_NULL_PARAMS;
		goto exit;
	}
	if ((client = mongoc_client_pool_pop(mpool->pool)) == NULL) {
		ret = MONGO_ERR_POOL_POP;
		goto exit;
	}
	if ((coll = mongoc_client_get_collection(client, db_name, 
						coll_name)) == NULL) {
		ret = MONGO_ERR_COLL_GET;
		mongoc_client_pool_push(mpool->pool, client);
		goto exit;
	}
	if (!mongoc_collection_insert(coll, MONGOC_INSERT_NONE, doc, 
								NULL, NULL)) {
		ret = MONGO_ERR_COLL_INSERT;
		mongoc_client_pool_push(mpool->pool, client);
		mongoc_collection_destroy(coll);
		goto exit;
	}
	mongoc_client_pool_push(mpool->pool, client);
	mongoc_collection_destroy(coll);
exit:
	return ret;
}

int mongo_doc_remove(mongo_context_pool_t *mpool, char *db_name, 
					char *coll_name, bson_t *query)
{
	int ret = 0;
	mongoc_client_t *client = NULL;
	mongoc_collection_t *coll = NULL;

	if (mpool == NULL || db_name == NULL || coll_name == NULL 
							|| query == NULL) {
		ret = MONGO_ERR_NULL_PARAMS;
		goto exit;
	}
	if ((client = mongoc_client_pool_pop(mpool->pool)) == NULL) {
		ret = MONGO_ERR_POOL_POP;
		goto exit;
	}
	if ((coll = mongoc_client_get_collection(client, db_name, 
						coll_name)) == NULL) {
		ret = MONGO_ERR_COLL_GET;
		mongoc_client_pool_push(mpool->pool, client);
		goto exit;
	}
	if (!mongoc_collection_remove(coll, MONGOC_REMOVE_NONE, query, 
								NULL, NULL)) {
		ret = MONGO_ERR_COLL_REMOVE;
		mongoc_client_pool_push(mpool->pool, client);
		mongoc_collection_destroy(coll);
		goto exit;
	}
	mongoc_client_pool_push(mpool->pool, client);
	mongoc_collection_destroy(coll);
exit:
	return ret;
}

int mongo_coll_create_ttl_index(mongo_context_pool_t *mpool, char *db_name, 
				char *coll_name, bson_t *keys, int32_t ttl)
{
	int ret = 0;
	mongoc_client_t *client = NULL;
	mongoc_collection_t *coll = NULL;
	mongoc_index_opt_t opt;

	if (mpool == NULL || db_name == NULL || coll_name == NULL 
							|| keys == NULL) {
		ret = MONGO_ERR_NULL_PARAMS;
		goto exit;
	}
	if (ttl < 0) {
		ret = MONGO_ERR_TTL_INVALID;
		goto exit;
	}
	if ((client = mongoc_client_pool_pop(mpool->pool)) == NULL) {
		ret = MONGO_ERR_POOL_POP;
		goto exit;
	}
	if ((coll = mongoc_client_get_collection(client, db_name, 
							coll_name)) == NULL) {
		ret = MONGO_ERR_COLL_GET;
		mongoc_client_pool_push(mpool->pool, client);
		goto exit;
	}
	mongoc_index_opt_init(&opt);
	opt.expire_after_seconds = ttl;
	if (!mongoc_collection_create_index(coll, keys,	&opt, NULL)) {
		ret = MONGO_ERR_COLL_INDEX_CREATE;
		mongoc_client_pool_push(mpool->pool, client);
		mongoc_collection_destroy(coll);
		goto exit;
	}
	mongoc_client_pool_push(mpool->pool, client);
	mongoc_collection_destroy(coll);
exit:
	return ret;
}

int mongo_doc_update(mongo_context_pool_t *mpool, char *db_name, 
			char *coll_name, bson_t *query, bson_t *update)
{
	int ret = 0;
	mongoc_client_t *client = NULL;
	mongoc_collection_t *coll = NULL;
	
	if (mpool == NULL || db_name == NULL || coll_name == NULL
			|| query == NULL || update == NULL) {
		ret = MONGO_ERR_NULL_PARAMS;
		goto exit;
	}
	if ((client = mongoc_client_pool_pop(mpool->pool)) == NULL) {
		ret = MONGO_ERR_POOL_POP;
		goto exit;
	}
	if ((coll = mongoc_client_get_collection(client, db_name, 
						coll_name)) == NULL) {
		ret = MONGO_ERR_COLL_GET;
		mongoc_client_pool_push(mpool->pool, client);
		goto exit;
	}
	if (!mongoc_collection_update(coll, MONGOC_UPDATE_MULTI_UPDATE, 
					query, update, NULL, NULL)) {
		ret = MONGO_ERR_COLL_UPDATE;
		mongoc_client_pool_push(mpool->pool, client);
		mongoc_collection_destroy(coll);
		goto exit;
	}
	mongoc_client_pool_push(mpool->pool, client);
	mongoc_collection_destroy(coll);
exit:
	return ret;
}

int mongo_doc_exists(mongo_context_pool_t *mpool, char *db_name, 
				char *coll_name, bson_t *query, bool *exists)
{
	int ret = 0;
	mongoc_client_t *client = NULL;
	mongoc_collection_t *coll = NULL;
	mongoc_cursor_t *cursor = NULL;
	const bson_t *doc = NULL;

	if (mpool == NULL || db_name == NULL || coll_name == NULL 
					|| query == NULL || exists == NULL) {
		ret = MONGO_ERR_NULL_PARAMS;
		goto exit;
	}
	if ((client = mongoc_client_pool_pop(mpool->pool)) == NULL) {
		ret = MONGO_ERR_POOL_POP;
		goto exit;
	}
	if ((coll = mongoc_client_get_collection(client, db_name, 
						coll_name)) == NULL) {
		ret = MONGO_ERR_COLL_GET;
		mongoc_client_pool_push(mpool->pool, client);
		goto exit;
	}
	if ((cursor = mongoc_collection_find(coll, MONGOC_QUERY_NONE, 
					0, 0, 0, query, NULL, NULL)) == NULL) {
		ret = MONGO_ERR_COLL_FIND;
		mongoc_client_pool_push(mpool->pool, client);
		mongoc_collection_destroy(coll);
		goto exit;
	}
	if (mongoc_cursor_next(cursor, &doc)) {
		*exists = true;
	} else {
		*exists = false;
	}
	mongoc_client_pool_push(mpool->pool, client);
        mongoc_collection_destroy(coll);
	mongoc_cursor_destroy(cursor);
exit:
	return ret;
}

char *mongo_err_msg(int code) 
{
	switch (code) {
	CASE(MONGO_ERR_NULL_PARAMS);
	CASE(MONGO_ERR_MEM_ALLOC);
	CASE(MONGO_ERR_POOL_PARAMS);
	CASE(MONGO_ERR_URI_NEW);
	CASE(MONGO_ERR_POOL_NEW);
	CASE(MONGO_ERR_POOL_POP);
	CASE(MONGO_ERR_COLL_GET);
	CASE(MONGO_ERR_COLL_FIND);
	CASE(MONGO_ERR_COLL_INSERT);
	CASE(MONGO_ERR_COLL_REMOVE);
	CASE(MONGO_ERR_TTL_INVALID);
	CASE(MONGO_ERR_COLL_INDEX_CREATE);
	default:
		return NULL;
	}
}

static int mongo_check_pool_params(int *min_size, int *max_size, 
					int *conn_timeout, int *op_timeout)
{
	int ret = 0;
	if (min_size == NULL || max_size == NULL 
		|| conn_timeout == NULL || op_timeout == NULL) {
		ret = MONGO_ERR_NULL_PARAMS;
		goto exit;
	}
	if (*min_size <= 0) {
		*min_size = DEFAULT_MIN_SIZE;
	}
	if (*max_size <= 0) {
		*max_size = DEFAULT_MAX_SIZE;
	}
	if (*min_size > *max_size) {
		*min_size = *max_size;
	}
	if (*conn_timeout <= 0) {
		*conn_timeout = DEFAULT_CONN_TIMEOUT;
	}
	if (*op_timeout <= 0) {
		*op_timeout = DEFAULT_OP_TIMEOUT;
	}
exit:
	return ret;
}
