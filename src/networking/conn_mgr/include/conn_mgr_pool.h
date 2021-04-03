/**@file 
 * @brief Header for using conn mgr pool APIs
 * */
#ifndef CONN_MGR_POOL_H
#define CONN_MGR_POOL_H
/*System Headers*/
#include <inttypes.h>
/*Local Headers*/
#include <conn_mgr_conn.h>

/**@typedef conn_mgr_pool_t
 * Data type for conn pool
 * */
typedef struct conn_mgr_pool_t conn_mgr_pool_t;

conn_mgr_pool_t *conn_mgr_pool_new(char *ip,
					uint16_t port, 
					uint32_t max_pool_size,
					uint32_t connect_timeout);
void conn_mgr_pool_destroy(conn_mgr_pool_t *pool);
conn_mgr_conn_t *conn_mgr_pool_pop(conn_mgr_pool_t *pool);
conn_mgr_conn_t *conn_mgr_pool_try_pop(conn_mgr_pool_t *pool);
void conn_mgr_pool_push(conn_mgr_pool_t *pool, conn_mgr_conn_t *conn);
#endif
