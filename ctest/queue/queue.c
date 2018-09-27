#include<stdio.h>
#include<stdlib.h>

#define QUEUE_SIZE 4

struct queue {
	int arr[QUEUE_SIZE];
	int head;
	int tail;
};

int queue_init(struct queue *q);
int queue_add(struct queue *q, int val);
int queue_delete(struct queue *q);
int queue_print(struct queue *q);

int main() 
{
	struct queue q;
	int ch;
	int val;

	if (queue_init(&q) < 0) {
		printf("Error in queue_init !!!\n");
		exit(1);
	}
	do {
		printf("1.Add\n2.Delete\n3.Print\n4.Quit\n\n");
		scanf("%d", &ch);
		switch (ch) {
		case 1 : 
			scanf("%d", &val);
			if (queue_add(&q, val) < 0) {
				printf("Error in queue_delete\n");
			}
			break;
		case 2 :
			if (queue_delete(&q) < 0) {
				printf("Error in queue_print\n");
			}
			break;
		case 3 : 
			if (queue_print(&q) < 0) {
				printf("Error in queue_add\n");
			}
			break;
		case 4 :
			break;
		default :
			printf("Wrong Choice !!!\n");
			break;
		}
	} while (ch != 4);

	return 0;
}

int queue_init(struct queue *q)
{
	if (q == NULL) {
		return -1;
	}

	q->head = -1;
	q->tail = -1;

	return 0;
}

int queue_add(struct queue *q, int val) 
{
	if (q == NULL) {
		return -1;
	}
	
	if ((q->tail + 1) % QUEUE_SIZE == q->head) {
		printf("Queue is full !!!\n");
		return 0;
	}
	if (q->head == -1 && q->tail == -1) { /*queue is empty*/
		q->head = 0;
		q->tail = 0;
	} else {
		q->tail = (q->tail + 1) % QUEUE_SIZE;
	}
	q->arr[q->tail] = val;
	return 0;
}

int queue_delete(struct queue *q) 
{
	if (q == NULL) {
		return -1;
	}
	if (q->head == -1) {
		printf("Queue is empty !!!\n");
		return 0;
	} else {
		printf("Deleting %d from queue\n", q->arr[q->head]);
		if (q->head == q->tail) {
			q->head = -1;
			q->tail = -1;
		} else {
			q->head = (q->head + 1) % QUEUE_SIZE;
		}

	}
	return 0;
}

int queue_print(struct queue *q) 
{
	int i = 0;

	if (q == NULL) {
		return -1;
	}
	if (q->head == -1) {
		printf("Queue is empty!!!\n");
		return 0;
	}
	i = q->head;
	while (1) {
	
		printf("%d ", q->arr[i]);
		if (i == q->tail) break;
		i = (i + 1) % QUEUE_SIZE;
	}
	printf("\n");
	return 0;
}
