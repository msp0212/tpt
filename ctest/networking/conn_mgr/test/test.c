/*System Headers*/
#include <stdio.h> 
#include <unistd.h> 
#include <pthread.h>
/*Local Headers*/
#include <conn_mgr_queue.h>
#include <conn_mgr_core.h>
#include <conn_mgr_conn.h>
#include <conn_mgr_pool.h>

#define NUM 10

#define HOST "172.19.13.25"
#define PORT 80
#define TIMEOUT 5000 /*ms*/
#define POOL_SIZE 2
#define NUM_WORKERS 4

struct pool_worker_info {
	int id;
	conn_mgr_pool_t *pool;
};

void test_queue(void);
void print_queue(void *data);
void queue_info(conn_mgr_queue_t *queue);
void test_conn(void);
void test_pool(void);
void *pool_worker(void *arg);

int main()
{
	test_queue();
	test_conn();
	test_pool();
	return 0;
}
/**Test connection pooling
 * Create a pool of connection
 * Spawn threads to get conn from pool, sleep for 2s 
 *	and then release the conn to pool
 * */
void test_pool(void)
{
	conn_mgr_pool_t *pool = NULL;
	struct pool_worker_info info[NUM_WORKERS];
	pthread_t tid[NUM_WORKERS];

	if ((pool = 
		conn_mgr_pool_new(HOST, PORT, POOL_SIZE, TIMEOUT)) == NULL) {
		printf("Error in conn_mgr_pool_new\n");
		goto exit;
	}
	printf("Conn mgr pool initialised\n");
	for (int i = 0; i < NUM_WORKERS; i++) {
		info[i].id = i;
		info[i].pool = pool;
		pthread_create(&tid[i], NULL, pool_worker, &info[i]);
	}
	for (int i = 0; i < NUM_WORKERS; i++) {
		pthread_join(tid[i], NULL);
		printf("Joined with thread %d\n", i);
	}
	pool_worker(&info[3]);
	conn_mgr_pool_destroy(pool);
exit:
	return;
}

void *pool_worker(void *arg)
{
	struct pool_worker_info *info = arg;
	conn_mgr_conn_t *conn;
	
	printf("Thread %d requested conn from pool\n", info->id);
	conn = conn_mgr_pool_pop(info->pool);
	if (conn == NULL) {
		printf("Error in conn_mgr_pool_pop\n");
		goto exit;
	}
	printf("Thread %d got conn from pool\n", info->id);
	sleep(2);
	conn_mgr_pool_push(info->pool, conn);
	printf("Thread %d released conn to pool\n", info->id);
exit:
	return NULL;
}
/**Test the connection to a given ip and port 
 * */
void test_conn(void)
{
	conn_mgr_conn_t *conn = NULL;
	int fd = 0;
	conn = conn_mgr_conn_new(HOST, PORT, TIMEOUT);
	if (conn == NULL) {
		printf("Error in %d %s in conn_mgr_conn_new\n", 
					conn_mgr_conn_errno,
					conn_mgr_err_msg(conn_mgr_conn_errno));
	} else {
		printf("Success in conn_mgr_conn_new\n");
	}
	fd = conn_mgr_conn_get_fd(conn);
	printf("fd for conn %d\n", fd);
	conn_mgr_conn_destroy(conn);
}

/**Function to test queue apis
 * */
void test_queue(void)
{
	int i = 0;
	int arr[NUM] = {0,};
	conn_mgr_queue_t q;
	int *d;
	conn_mgr_queue_init(&q);
	for (i = 0; i < NUM; i++) {
		arr[i] = i;
		if (i&1) {
			conn_mgr_queue_push_head(&q, &arr[i]);
		} else {
			conn_mgr_queue_push_tail(&q, &arr[i]);
		}
	}
	queue_info(&q);
	for (i = 0; i < NUM; i++) {
		queue_info(&q);
		d = (int *)conn_mgr_queue_pop_head(&q);
		printf("data popped = %d\n", *d);
	}
	return;
}

void print_queue(void *data) 
{
	int *d = (int *)data;
	printf("%d ", *d);
	return;
}

void queue_info(conn_mgr_queue_t *queue)
{
	uint32_t count = 0;

	count = conn_mgr_queue_get_length(queue);
	printf("q len = %u\n", count);
	printf("queue = ");
	conn_mgr_queue_print(queue, print_queue);
	printf("\n");
}
