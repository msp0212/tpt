#ifndef MONGO_API_H
#define MONGO_API_H
#include <libbson-1.0/bson.h>

typedef struct mongo_context_pool mongo_context_pool_t;

void mongo_api_init();
void mongo_api_cleanup();
int mongo_context_pool_init(mongo_context_pool_t **mpool, char *ip, int port,
					int min_size, int max_size,
					int conn_timeout, int op_timeout);
int mongo_context_pool_destroy(mongo_context_pool_t **mpool);
int mongo_insert_doc(mongo_context_pool_t *mpool, char *db_name,
                                                char *coll_name, bson_t *doc);
int mongo_doc_remove(mongo_context_pool_t *mpool, char *db_name,
                                        char *coll_name, bson_t *query);
int mongo_coll_create_ttl_index(mongo_context_pool_t *mpool, char *db_name,
                                char *coll_name, bson_t *keys, int32_t ttl);
int mongo_doc_exists(mongo_context_pool_t *mpool, char *db_name,
                                char *coll_name, bson_t *query, bool *exists);
int mongo_doc_update(mongo_context_pool_t *mpool, char *db_name,
                        char *coll_name, bson_t *query, bson_t *update);
char *mongo_err_msg(int code);
#endif
