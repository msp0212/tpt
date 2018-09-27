#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<fcntl.h>
#include<semaphore.h>
#include<errno.h>
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

int queue_init(struct queue *q);
int queue_add(struct queue *q, int val);
int queue_print(struct queue *q);

void signal_handler(int signum);

int main() 
{
	struct sigaction sa;
	int val = 0;

	/*register signal handler*/
	sa.sa_handler = signal_handler;
	if(sigaction(SIGINT, &sa, NULL)) {
		printf("Error in registering signal");
		exit(1);
	}
	/*get shm key*/
	shm_key = ftok(PATH, PROJ_ID);
	if (shm_key < 0) {
		printf("Error [%d] [%s] in ftok\n", errno, strerror(errno));
		exit(1);
	}
	printf("shm_key is [%d]\n", shm_key);

	/*get a shared memory segment*/
	shm_id = shmget(shm_key, sizeof(struct queue), 
				IPC_CREAT | IPC_EXCL | 0666);
	if (shm_id < 0) {
		if(errno == EEXIST) {
			printf("Shared memory segment exists...\n");
			shm_id = shmget(shm_key, sizeof(struct queue), 
							IPC_CREAT | 0666);
			if (shm_id < 0) {
				printf("Error [%d] [%s] in shmget", 
						errno, strerror(errno));
				exit(1);
			}

		} else {
			printf("Error [%d] [%s] in shmget",
						errno, strerror(errno));
			exit(1);
		}
	}
	printf("shm_id is [%d]\n\n", shm_id);
	
	/*attach shared memory to process's address space*/
	q = shmat(shm_id, NULL, 0);
	if (q == (void *)-1) {
		printf("Error [%d] [%s] in shmat", errno, strerror(errno));
		exit(1);
	}
	queue_init(q);
	full = sem_open("/test_full", O_CREAT, 0666, 0);
	empty = sem_open("/test_empty", O_CREAT, 0666, QUEUE_SIZE);
	mutex = sem_open("/test_mutex", O_CREAT, 0666, 1);

	/*producer loop*/
	while(1) {
		val = rand();
		printf("Producer produced %d\n", val);
		printf("Waiting for an empty slot in buffer\n");
		sem_wait(empty);
		printf("Waiting for access to critical section\n");
		sem_wait(mutex);
		queue_add(q, val);
		queue_print(q);
		sem_post(mutex);
		sem_post(full);
		printf("Production successfull\n\n");
	}
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

