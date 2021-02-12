#include<stdio.h>
#include<gperftools/tcmalloc.h>
#include<workq.h>
#include<time.h>
#include<numa.h>

#define NUM_TIMES 100000 /*1 million*/
#define SMALL 1024 /*1KB*/
#define MEDIUM 1048576 /*1MB*/
#define BIG 10485760 /*10MB*/ 

WorkQT gMyWorkQ;
void doSomething();
int main()
{
	int i;
	/*intialize the workq*/
	if(WorkQInit(&gMyWorkQ, 5, 500, (void *)doSomething )	!= 0)
	{
		fprintf(stderr,"%s: Error intialising workq !!!\n",__FUNCTION__);
		exit(1);
	}
	for(i=0;i<NUM_TIMES;i++)
	{
		/*start creating threads*/
		if(WorkQAdd(&gMyWorkQ, doSomething) != 0)
		{
			fprintf(stderr,"%s : Error addding to workq !!!\n", __FUNCTION__);
		}
	}
	pthread_exit(NULL);
}

void doSomething()
{
	char *myStr = NULL;
	int i;
	numa_run_on_node(0);
	numa_set_preferred(0);
	for(i=0;i<1;i++)
	{
	/*small allocation*/
	myStr = (char*)tc_malloc(SMALL*sizeof(char));
	strcpy(myStr,"mohit singh");
	strcpy(myStr,"aaaaaaaaaaa");
	tc_free(myStr); myStr = NULL;
	/*medium allocation*/
	myStr = (char*)tc_malloc(MEDIUM*sizeof(char));
	strcpy(myStr,"mohit singh");
	strcpy(myStr,"aaaaaaaaaaa");
	tc_free(myStr); myStr = NULL;
	/*big allocation*/
	myStr = (char*)tc_malloc(BIG*sizeof(char));
	strcpy(myStr,"mohit singh");
	strcpy(myStr,"aaaaaaaaaaa");
	tc_free(myStr); myStr = NULL;
	}
//	printf("Done\n");
	return;
} 
