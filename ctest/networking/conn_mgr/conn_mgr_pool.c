/*System Headers*/
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
/*Local Headers*/
#include <conn_mgr_pool.h>
#include <conn_mgr_queue.h>
#include <conn_mgr_errors.h>


/**@struct conn_mgr_pool_t
 * @brief This structure contains the pool information*/
struct conn_mgr_pool_t {
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	char ip[16];
	uint16_t port;
	uint32_t max_pool_size;
	uint32_t size;
	uint32_t connect_timeout;
	conn_mgr_queue_t queue;
};

/**@brief get a conn pool object
 * @param[in] ip ip to connect
 * @param[in] port port to connect
 * @param[in] max_pool_size max no of conn in the pool
 * @param[in] connect_timeout connect timeout in ms
 * @return On success, pointer to newly allocated conn pool object.
 *	   On failure, NULL
 * */
conn_mgr_pool_t *conn_mgr_pool_new(char *ip,
					uint16_t port,
					uint32_t max_pool_size,
					uint32_t connect_timeout)
{
	conn_mgr_pool_t *pool = NULL;

	if (ip == NULL) {
		goto exit;
	}
	if ((pool = malloc(sizeof (*pool))) == NULL) {
		goto exit;
	}
	pthread_mutex_init(&pool->mutex, NULL);
	pthread_cond_init(&pool->cond, NULL);
	strncpy(pool->ip, ip, sizeof (pool->ip));
	pool->port = port;
	pool->max_pool_size = max_pool_size;
	pool->size = 0;
	pool->connect_timeout = connect_timeout;
	conn_mgr_queue_init(&pool->queue);
exit:
	return pool;
}

/**@brief destroy the conn pool object
 * @param[in] pool pointer to conn pool object
 * */
void conn_mgr_pool_destroy(conn_mgr_pool_t *pool)
{
	conn_mgr_conn_t *conn = NULL;

	if (pool == NULL) {
		goto exit;
	}
	while ((conn = conn_mgr_queue_pop_head(&pool->queue)) != NULL) {
		conn_mgr_conn_destroy(conn);
	}
	pthread_mutex_destroy(&pool->mutex);
	pthread_cond_destroy(&pool->cond);
	free(pool);
exit: 
	return;
}

/**@brief pop a conn from the conn pool
 * @param[in] pool pointer to conn pool object
 * @return On success, pointer to a conn object. Call will block till there is a
 *		free conn in the pool
 *	   On failure, NULL
 * */
conn_mgr_conn_t *conn_mgr_pool_pop(conn_mgr_pool_t *pool)
{
	conn_mgr_conn_t *conn = NULL;

	if (pool == NULL) {
		goto exit;
	}
	pthread_mutex_lock(&pool->mutex);
	while ((conn = conn_mgr_queue_pop_head(&pool->queue)) == NULL) {
		if (pool->size < pool->max_pool_size) {
			conn = conn_mgr_conn_new(pool->ip, pool->port,
							pool->connect_timeout);
			if (conn != NULL)
				pool->size++;
			break;
		} else {
			pthread_cond_wait(&pool->cond, &pool->mutex);
		}
	}
	pthread_mutex_unlock(&pool->mutex);
exit :
	return conn;
}

/**@brief pop a conn from the conn pool
 * @param[in] pool pointer to conn pool object
 * @return On success, pointer to a conn object. Call will immediately return 
 *		if there is no free conn in the pool and NULL is returned
 * */
conn_mgr_conn_t *conn_mgr_pool_try_pop(conn_mgr_pool_t *pool)
{
	conn_mgr_conn_t *conn = NULL;
	if (pool == NULL) {
		goto exit;
	}
	pthread_mutex_lock(&pool->mutex);
	while ((conn = conn_mgr_queue_pop_head(&pool->queue)) == NULL) {
		if (pool->size < pool->max_pool_size) {
			conn = conn_mgr_conn_new(pool->ip, pool->port,
							pool->connect_timeout);
			pool->size++;
		}
	}
	pthread_mutex_unlock(&pool->mutex);
exit :
	return conn;

}

/**@brief push a conn to the conn pool
 * @param[in] pool pointer to conn pool object
 * @param[in] conn pointer to conn object
 * */
void conn_mgr_pool_push(conn_mgr_pool_t *pool, conn_mgr_conn_t *conn)
{
	if (pool == NULL) {
		goto exit;
	}
	pthread_mutex_lock(&pool->mutex);
	conn_mgr_queue_push_head(&pool->queue, conn);
	pthread_cond_signal(&pool->cond);
	pthread_mutex_unlock(&pool->mutex);
exit:
	return;
}
