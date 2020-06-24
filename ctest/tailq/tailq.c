#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>

struct log_object {
	int log_id;
	char log_msg[32];
	TAILQ_ENTRY(log_object) log_q_entry;
};

#define MAX_OBJS 5

TAILQ_HEAD(log_object_q_head, log_object);

void tailq_create_tail(struct log_object_q_head *head) 
{
	struct log_object *obj;
	int i; 

	/*tailq insertion at tail*/
	for (i = 0; i < MAX_OBJS; i++) {
		obj = malloc(sizeof(*obj));
		obj->log_id = i;
		snprintf(obj->log_msg, sizeof(obj->log_msg), "this log msg # %d", i);
		TAILQ_INSERT_TAIL(head, obj, log_q_entry);
	}
	return;
}

void tailq_create_head(struct log_object_q_head *head) 
{
	struct log_object *obj;
	int i; 

	/*tailq insertion at tail*/
	for (i = 0; i < MAX_OBJS; i++) {
		obj = malloc(sizeof(*obj));
		obj->log_id = i;
		snprintf(obj->log_msg, sizeof(obj->log_msg), "this log msg # %d", i);
		TAILQ_INSERT_HEAD(head, obj, log_q_entry);
	}
	return;
}

void tailq_traverse(struct log_object_q_head *head)
{
	struct log_object *iter;
	/*tailq traversal*/
	printf("\n");
	TAILQ_FOREACH(iter, head, log_q_entry) {
		printf("log - %d | %s\n", iter->log_id, iter->log_msg);
	}
	return;
}

void tailq_traverse_reverse(struct log_object_q_head *head)
{
	struct log_object *iter;
	/*tailq traversal reverse*/
	printf("\n");
	TAILQ_FOREACH_REVERSE(iter, head, log_object_q_head, log_q_entry) {
		printf("log - %d | %s\n", iter->log_id, iter->log_msg);
	}
	return;
}

void tailq_destroy_head(struct log_object_q_head *head)
{
	struct log_object *iter;
	/*tailq removal*/
	while (!TAILQ_EMPTY(head)) {
		iter = TAILQ_FIRST(head);
		TAILQ_REMOVE(head, iter, log_q_entry);
		free(iter);
	}
	return;
}

void tailq_destroy_tail(struct log_object_q_head *head)
{
	struct log_object *iter;
	/*tailq removal*/
	while (!TAILQ_EMPTY(head)) {
		iter = TAILQ_LAST(head, log_object_q_head);
		TAILQ_REMOVE(head, iter, log_q_entry);
		free(iter);
	}
	return;
}

void tailq_traverse_from_entry(struct log_object_q_head *head,
										struct log_object *obj)
{
	struct log_object *iter = obj;
	printf("\n");
	while (iter != NULL) {
		printf("log - %d | %s\n", iter->log_id, iter->log_msg);
		iter = TAILQ_NEXT(iter, log_q_entry);
	}
}

void tailq_traverse_reverse_from_entry(struct log_object_q_head *head,
										struct log_object *obj)
{
	struct log_object *iter = obj;
	printf("\n");
	while (iter != NULL) {
		printf("log - %d | %s\n", iter->log_id, iter->log_msg);
		iter = TAILQ_PREV(iter, log_object_q_head, log_q_entry);
	}
}

int main()
{
	struct log_object_q_head log_obj_q;

	TAILQ_INIT(&log_obj_q);

	printf("test1\n");
	tailq_create_tail(&log_obj_q);
	tailq_traverse(&log_obj_q);
	tailq_traverse_reverse(&log_obj_q);
	tailq_destroy_head(&log_obj_q);


	printf("test2\n");
	tailq_create_head(&log_obj_q);
	tailq_traverse(&log_obj_q);

	{
		struct log_object *obj = TAILQ_FIRST(&log_obj_q);
		obj = TAILQ_NEXT(obj, log_q_entry);
		tailq_traverse_from_entry(&log_obj_q, obj);
	}

	{
		struct log_object *obj = TAILQ_LAST(&log_obj_q, log_object_q_head);
		obj = TAILQ_PREV(obj, log_object_q_head, log_q_entry);
		tailq_traverse_reverse_from_entry(&log_obj_q, obj);
	}

	tailq_destroy_tail(&log_obj_q);

	return 0;
}
