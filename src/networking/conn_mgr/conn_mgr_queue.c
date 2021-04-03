/*System Headers*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*Local Headers*/
#include <conn_mgr_queue.h>
#include <conn_mgr_errors.h>

/**@brief Init the queue
 * @param[in] queue pointer to a queue object
 * */
void conn_mgr_queue_init(conn_mgr_queue_t *queue) 
{
	if (queue == NULL) {
		goto exit;
	}
	memset(queue, 0, sizeof(*queue));
exit :
	return;
}

/**@brief push data at head of the queue
 * @param[in] queue pointer to queue object
 * @param[in] data pointer to data to be pushed
 * */
void conn_mgr_queue_push_head(conn_mgr_queue_t *queue, void *data)
{
	conn_mgr_queue_item_t *item = NULL;

	if (queue == NULL || data == NULL) {
		goto exit;
	}
	if ((item = malloc(sizeof(*item))) == NULL) {
		goto exit;
	}
	item->data = data;
	item->next = queue->head;
	queue->head = item;
	if (queue->tail == NULL) {
		queue->tail = item;
	}
exit:
	return;
}

/**@brief push data at tail of the queue
 * @param[in] queue pointer to queue object
 * @param[in] data pointer to data to be pushed
 * */
void conn_mgr_queue_push_tail(conn_mgr_queue_t *queue, void *data)
{
	conn_mgr_queue_item_t *item = NULL;

	if (queue == NULL || data == NULL) {
		goto exit;
	}
	if ((item = malloc(sizeof(*item))) == NULL) {
		goto exit;
	}
	item->data = data;
	item->next = NULL;
	if (queue->tail != NULL) {
		queue->tail->next = item;;
	} else {
		queue->head = item;
	}
	queue->tail = item;
exit:
	return;
}

/**@brief pop data from head of the queue
 * @param[in] queue pointer to queue object
 * @return On success, pointer to data popped from the queue
 *	   on failure, NULL
 * */
void *conn_mgr_queue_pop_head(conn_mgr_queue_t *queue)
{
	void *data = NULL;
	conn_mgr_queue_item_t *item = NULL;

	if (queue == NULL) {
		goto exit;
	}
	if ((item = queue->head)) {
		if (item->next == NULL) {
			queue->tail = NULL;
		}
		data = item->data;
		queue->head = item->next;
		free(item);
	}
exit :
	return data;
}

/**@brief get the length of the queue
 * @param[in] queue pointer to queue object
 * @return length of the queue
 * */
uint32_t conn_mgr_queue_get_length(conn_mgr_queue_t *queue)
{
	uint32_t count = 0;
	conn_mgr_queue_item_t *item = NULL;

	for (item = queue->head; item != NULL; item = item->next) {
		count++;
	}
	return count;
}

/**@brief print the items in the queue
 * @param[in] queue pointer to queue object
 * @param[in] print_callback pointer to a callback function.
 *		This function in called for each queue item 
 *			and item is passed as an argument to it.
 * */
void conn_mgr_queue_print(conn_mgr_queue_t *queue, 
				void (*print_callback)(void *))
{
	conn_mgr_queue_item_t *item = NULL;

	if (print_callback == NULL) {
		goto exit;
	}
	for (item = queue->head; item != NULL; item = item->next) {
		(*print_callback)(item->data);
	}
exit:
	return;
}
