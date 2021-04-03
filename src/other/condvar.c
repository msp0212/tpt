#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>

#define count_limit 12
#define tcount 10
#define numthreads 3

pthread_mutex_t mutex_count;
pthread_cond_t cv_count;
int count=0;

void* incrementCount(void *arg)
{
  long tid=(long)arg;
  int i;
  
  for(i=0;i<tcount;i++)
  {
    pthread_mutex_lock(&mutex_count);
    count++;
    if(count==count_limit)
    {
      pthread_cond_signal(&cv_count);
      printf("IncrementCount::Thread#%ld::count=%d.....Threshold reahced\n",tid,count);
    }
    printf("IncrementCount::Thread#%ld::count=%d now \n",tid,count);
    pthread_mutex_unlock(&mutex_count);
    sleep(1);
  }
  pthread_exit(NULL);
}

void* watchCount(void *arg)
{
    long tid=(long)arg;

    printf("WatchCount::Thread#%ld::count now=%d\n",tid,count);
    pthread_mutex_lock(&mutex_count);

    while(count<count_limit)
    {
        pthread_cond_wait(&cv_count,&mutex_count);
        printf("WatchCount::Thread%ld::count now=%d SIGNAL RECIEVED!!!\n",tid,count);
        count=count+100;
        printf("WatchCount::Thread%ld count now=%d!!!\n",tid,count);
    }
    pthread_mutex_unlock(&mutex_count);
    pthread_exit(NULL);
}



int main()
{
  int i,rc;
  long t1=1,t2=2,t3=3;

  pthread_attr_t attr;
  pthread_t threads[3];

  pthread_mutex_init(&mutex_count,NULL);
  pthread_cond_init(&cv_count,NULL);

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);

  pthread_create(&threads[0],NULL,watchCount,(void*)t1);
  pthread_create(&threads[1],NULL,incrementCount,(void*)t2);
  pthread_create(&threads[2],NULL,incrementCount,(void*)t3);

  for(i=0;i<numthreads;i++)
  {
     pthread_join(threads[i],NULL);
  }
  printf("Main::Completed Successfully !!!\n");
  pthread_exit(NULL);
}
