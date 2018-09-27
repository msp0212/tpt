/** @file 
 * @brief Header file for using conn mgr queue APIs 
 * */
#ifndef CON_MGR_QUEUE_H
#define CONN_MGR_QUEUE_H
#include <inttypes.h>

/**@typedef conn_mgr_queue_item_t
 * @brief Data type for an item in the queue
 * */
typedef struct conn_mgr_queue_item_t conn_mgr_queue_item_t;

/**@struct conn_mgr_queue_item_t 
 * @brief This structure describes an item in the queue
 * */
struct conn_mgr_queue_item_t {
	void *data;
	conn_mgr_queue_item_t *next;
};

/**@typedef conn_mgr_queue_t 
 * @brief Data type for queue
 * */
typedef struct conn_mgr_queue_t conn_mgr_queue_t;

/**@struct conn_mgr_queue_t
 * @brief The queue structure with head and tail pointers
 * */
struct conn_mgr_queue_t {
	conn_mgr_queue_item_t *head;
	conn_mgr_queue_item_t *tail;
};

void conn_mgr_queue_init(conn_mgr_queue_t *queue);
void *conn_mgr_queue_pop_head(conn_mgr_queue_t *queue);
void conn_mgr_queue_push_head(conn_mgr_queue_t *queue, void *data);
void conn_mgr_queue_push_tail(conn_mgr_queue_t *queue, void *data);
uint32_t conn_mgr_queue_get_length(conn_mgr_queue_t *queue);
void conn_mgr_queue_print(conn_mgr_queue_t *queue, 
				void (print_callback)(void *));
#endif
