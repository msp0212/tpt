#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<pthread.h>
#include"icap_client.h"

#define POOL_SIZE 1

typedef struct threadData
{
	icapContext *context;
	pHandleIcapData fnPtr;
	int client_socket;
	int *icapSessionValid;
}ThreadData;

int withCallback = 1;

int handleIcapData(const char *respData, int respDataLen,int clientSocket)
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
	char *buffer = NULL;
	int bufLen	= 0; 
	if(withCallback)
	{
		ret = icapReceiveData(tData->context, 1 , NULL, NULL , tData->fnPtr, tData->client_socket);
		if(ret < 0)
		{
			printf("%s : Error [%d] [%s]\n", __FUNCTION__, ret,icapGetErrorString(ret));
			return;
		}
	}
	else
	{
		ret = icapReceiveData(tData->context, 0 , &buffer, &bufLen, NULL, 0);
		if(ret < 0)
		{
			printf("%s : Error [%d] [%s]\n", __FUNCTION__, ret,icapGetErrorString(ret));
			return;
		}
		printf("%s : DataLength [%d]\n[%s]\n", __FUNCTION__, bufLen, buffer);	
	}
	printf("%s : status code = [%d]\n", __FUNCTION__, ret);
	free(arg);arg=NULL;
	return;
}
int main()
{
	int ret, port = 1344;
	char ip[16] = "182.18.161.65", *response = NULL;
	icapContext *context = NULL;
	pthread_t tReceiver;
	ThreadData *tData = NULL;

	ret = icapInitContextPool(POOL_SIZE);
	if(ret < 0)
	{
		printf("Error [%d] [%s]\n",ret ,icapGetErrorString(ret));
		exit(1);
	}
	/*connect to icap server*/
	ret = icapConnect(ip, port, &context);
	if(ret < 0)
	{
		printf("Error [%d] [%s]\n",ret ,icapGetErrorString(ret));
		exit(1);
	}
	printf("Connected to ICAP server successfully...\n\n");

	/*response modification*/
	char httpRequestHeaders1[] = "GET /origin-resource HTTP/1.1\r\n"
		"Host: www.origin-server.com\r\n"
		"Accept: text/html, text/plain, image/gif\r\n"
		"Accept-Encoding: gzip, compress\r\n"
		"\r\n";

	tData = (ThreadData*)calloc(1,sizeof(ThreadData));
	tData->context = context;
	tData->fnPtr = handleIcapData;
	tData->client_socket = 0;
	pthread_create(&tReceiver,NULL,receiver,tData);
	ret = icapSendHeaders(context, "/", "", ICAP_MOD_REQ, httpRequestHeaders1 , NULL, 0, 1, -1);
	if(ret < 0)
	{
		printf("Error [%d] [%s]\n", ret,icapGetErrorString(ret));
		exit(1);
	}
	printf("\nHeaders Sent\n");
	pthread_join(tReceiver,NULL);
	/*disconnect from icap server*/
	ret = icapDisconnect(&context);
	if(ret < 0)
	{
		printf("ERROR disconnecting\n");
	}
	ret = icapFreeContextPool();
	if(ret < 0)
	{
		printf("Error [%d] [%s]\n",ret ,icapGetErrorString(ret));
		exit(1);
	}
	return 0;
}

