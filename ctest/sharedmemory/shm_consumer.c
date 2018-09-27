#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<errno.h>
#include<fcntl.h>
#include<semaphore.h>
#include<unistd.h>
#include<signal.h>

#define PATH "/tmp"
#define PROJ_ID 1
#define QUEUE_SIZE 4

struct queue {
	int arr[QUEUE_SIZE];
	int head;
	int tail;
};

sem_t *full;
sem_t *empty;
sem_t *mutex;
key_t shm_key;
int shm_id;
struct queue *q = NULL;

int queue_delete(struct queue *q);
int queue_print(struct queue *q);
void signal_handler(int signum);

int main() 
{
	struct sigaction sa;	
	
	/*register signal handler*/
	sa.sa_handler = signal_handler;
	if(sigaction(SIGINT, &sa, NULL)) {
		printf("Error in registering signal");
		exit(1);
	}

	shm_key = ftok(PATH, PROJ_ID);
	if (shm_key < 0) {
		printf("Error [%d] [%s] in ftok\n", errno, strerror(errno));
		exit(1);
	}
	printf("shm_key is [%d]\n", shm_key);

	shm_id = shmget(shm_key, sizeof(struct queue), 0666);
	if (shm_id < 0) {
		printf("Error [%d] [%s] in shmget\n", errno, strerror(errno));
		exit(1);
	}
	printf("shm_id is [%d]\n", shm_id);

	q = shmat(shm_id, NULL, 0);
	if (q == (void *)-1) {
		printf("Error [%d] [%s] in shmat", errno, strerror(errno));
		exit(1);
	}
	full = sem_open("/test_full", O_CREAT, 0666, 0);
	empty = sem_open("/test_empty", O_CREAT, 0666, QUEUE_SIZE);
	mutex = sem_open("/test_mutex", O_CREAT, 0666, 1);
	/*consumer loop*/
	while(1) {
		printf("Waiting for buffer to get filled\n");
		sem_wait(full);
		printf("Waiting for access to critical section\n");
		sem_wait(mutex);
		queue_delete(q);
		queue_print(q);
		sem_post(mutex);
		sem_post(empty);
		printf("Consumption  successfull\n\n");
		sleep(1);
	}


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

void signal_handler(int signum)
{
	printf("Received signal [%d] [%s]\n", signum, strsignal(signum));
	if (shmdt(q) < 0) {
		printf("Error [%d] [%s] in shmdt\n", errno, strerror(errno));
	}
	/*
	if (shmctl(shm_id, IPC_RMID, NULL) < 0) {
		printf("Error [%d] [%s] in shmctl\n", errno, strerror(errno));
	}*/
	signal(signum, SIG_DFL);
	raise(signum);
	return ;
}

