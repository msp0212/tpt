#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"icap_client.h"

#define POOL_SIZE 5
int testInitPool();
int testFreePool();

typedef struct threadData
{
  icapContext *context;
  pHandleIcapData fnPtr;
	int client_socket;
	int *icapSessionValid;
}ThreadData;
int handleIcapData(const char *respData, int respDataLen, int client_socket)
{
  char *data = NULL;
  if(respData == NULL)
  {
    printf("%s : NO DATA received\n", __FUNCTION__);
    return 1;
  }
  data = (char*)calloc(respDataLen+1, sizeof(char));
  memcpy(data, respData, respDataLen);
  *(data+respDataLen) = '\0';
  printf("%s : DataLength [%d]\n[%s]\n", __FUNCTION__, respDataLen, data);
  free(data);data = NULL;
  return 0;
}

void *receiver(void *arg)
{
  int ret;
  ThreadData *tData = (ThreadData*)arg;
  ret = icapReceiveData(tData->context, tData->fnPtr, tData->client_socket);
  if(ret < 0)
  {
    printf("Error [%d] [%s]\n", ret,icapGetErrorString(ret));
    return;
  }
  printf("%s : status code = [%d]\n", __FUNCTION__, ret);
  free(arg);arg=NULL;
  return;
}

int main()
{
	int ret;
	ret = testInitPool();
	if(ret < 0)
	{
		printf("testInitPool failed...Exiting!!!\n");
		exit(1);
	}
	else
	{
		printf("testInitPool passed.\n");
	}
	
	ret = test1();
	if(ret < 0)
	{
		printf("test1 failed...Exiting!!!\n");
		exit(1);
	}
	else
	{
		printf("test1 passed.\n");
	}
	ret = test2();
	if(ret < 0)
	{
		printf("test2 failed...Exiting!!!\n");
		exit(1);
	}
	else
	{
		printf("test2 passed.\n");
	}

	ret = testFreePool();
	if(ret < 0)
	{
		printf("testFreePool failed...Exiting!!!\n");
		exit(1);
	}
	else
	{
		printf("testFreePool passed.\n");
	}
	return 0;
}

int testInitPool() /*test : init the connection pool*/
{
	int ret;
	ret = icapInitContextPool(POOL_SIZE);
	if(ret < 0)	
	{
		fprintf(stderr,"%s : Error [%d] [%s]\n", __FUNCTION__, ret, icapGetErrorString(ret));
		return -1;
	}
	return 0;
}
int testFreePool()
{
	int ret;
	ret = icapFreeContextPool();
	if(ret < 0)	
	{
		fprintf(stderr,"%s : Error [%d] [%s]\n", __FUNCTION__, ret, icapGetErrorString(ret));
		return -1;
	}
	return 0;
}

int test1() /*test : connect, send OPTIONS request and disconnect*/
{
	int ret;
	pthread_t tReceiver;
  ThreadData *tData = NULL;
	icapContext *context = NULL;
	ret = icapConnect("172.19.3.25",1344, &context);
	if(ret < 0)	
	{
		fprintf(stderr,"%s : Error [%d] [%s]\n", __FUNCTION__, ret, icapGetErrorString(ret));
		return -1;
	}
	ret = icapSendOptionsRequest(context, "/echo");
	if(ret < 0)	
	{
		fprintf(stderr,"%s : Error [%d] [%s]\n", __FUNCTION__, ret, icapGetErrorString(ret));
		return -1;
	}
	tData = (ThreadData*)calloc(1,sizeof(ThreadData));
  tData->context = context;
  tData->fnPtr = handleIcapData;
  pthread_create(&tReceiver,NULL,receiver,tData);
  pthread_join(tReceiver,NULL);

	ret = icapDisconnect(&context);
	if(ret < 0)	
	{
		fprintf(stderr,"%s : Error [%d] [%s]\n", __FUNCTION__, ret, icapGetErrorString(ret));
		return -1;
	}
}

int test2() /*test : pool limit, remove a context from active list and add to free list*/
{
	int i,ret,flag = 0;
	icapContext *context[POOL_SIZE+1] = {NULL,} ;
	for(i=0;i<POOL_SIZE+1;i++)
	{
		ret = icapConnect("172.19.3.25",1344,&context[i]);/*should give err on 6th iteration*/
		if(ret < 0)
  	{
			if(ret == ICAP_ERR_TOO_MANY_CONNECTIONS)
			{
				flag = 1;
			}
    	fprintf(stderr,"%s : Error [%d] [%s]\n", __FUNCTION__, ret, icapGetErrorString(ret));
		}
  }
	icapDisconnect(&context[0]);/*remove head*/
	icapDisconnect(&context[4]);/*remove last*/
	ret = icapConnect("172.19.3.25",1344,&context[0]);/*should connect here*/
	if(ret < 0)
	{
		fprintf(stderr,"%s : Error [%d] [%s]\n", __FUNCTION__, ret, icapGetErrorString(ret));
	}
	ret = icapConnect("172.19.3.25",1344,&context[4]);/*should connect here*/
	if(ret < 0)
	{
		fprintf(stderr,"%s : Error [%d] [%s]\n", __FUNCTION__, ret, icapGetErrorString(ret));
	}

	for(i=0;i<POOL_SIZE+1;i++)
	{
		ret = icapDisconnect(&context[i]);/*should give invalid params error on 6th iteration*/
		if(ret < 0)
  	{
   		fprintf(stderr,"%s : Error [%d] [%s]\n", __FUNCTION__, ret, icapGetErrorString(ret));
		}
	}
  	if(flag == 0)
			return -1;
		return 0;
}

