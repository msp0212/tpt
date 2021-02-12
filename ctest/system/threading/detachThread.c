#include<stdio.h>
#include<pthread.h>

void *worker();
int main()
{
	pthread_t thWorker;
	pthread_create(&thWorker, NULL, worker, NULL);
	pthread_detach(thWorker);
	pthread_exit(NULL);
	return 0;
}

void *worker()
{
	int num = 10;
	while(num)
	{
		printf("%s : Inside worker thread !!!\n", __FUNCTION__);
		num--;
		sleep(2);
	}
	return;
}
