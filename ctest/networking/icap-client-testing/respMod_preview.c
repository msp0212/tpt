#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<pthread.h>
#include"icap_client.h"

#define POOL_SIZE 1
#define ICAP_CHUNK_END "0\r\n\r\n"
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
				int ret, port = 1344,i;
				char ip[16] = "172.19.3.25", *response = NULL;
				icapContext *context = NULL;
				pthread_t tReceiver;
				ThreadData *tData = NULL;
				char *preview = NULL;
				int previewSize = 1024;
				char *chunkedData = NULL;
				int chunkLen = 0;
				
				ret = icapInitContextPool(POOL_SIZE);
				if(ret < 0)
				{
								printf("Error [%d] [%s] in icapInitContextPool\n",ret ,icapGetErrorString(ret));
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
								"Accept: text/html, text/plain, image/gif\r\n\r\n";
				char httpResponseHeaders[] = "HTTP/1.1 200 OK\r\n"
								"Server: Apache/1.3.6(Unix)\r\n"
								"Content-Type: text/plain\r\n\r\n";
				char httpResponseBody1[]  ="d\r\n"
								"Some Content1\r\n";

				char httpResponseBody2[]  = "d\r\n"
								"Some Content2\r\n";
				char httpResponseBody3[] ="0\r\n\r\n";

				preview = (char*)calloc(previewSize, sizeof(char));
				for(i=0;i<previewSize;i++)
				{
					preview[i]='M';
				}
				ret = formChunkedData(preview, previewSize, &chunkedData, &chunkLen);
				if(ret < 0)
        {
                printf("Error [%d] [%s] in formChunkedData \n", ret,icapGetErrorString(ret));
                exit(1);
        }
				ret = icapSendHeaders(context, "/echo", "", ICAP_MOD_RESP, httpRequestHeaders1 , httpResponseHeaders, 1, 1, 1024);
				if(ret < 0)
				{
								printf("Error [%d] [%s] in icapSendHeaders\n", ret,icapGetErrorString(ret));
								exit(1);
				}
				printf("Headers Sent\n");
				ret = icapSendPreviewAndWait(context, chunkedData, chunkLen, &preview, &previewSize);
				if(ret < 0)
        {
                printf("Error [%d] [%s] in icapSendPreviewAndWait\n", ret,icapGetErrorString(ret));
                exit(1);
        }
				printf("StatusCode from ICAP server after preview [%d]\n", ret);
				if(ret != 100)
				{
					exit(1);
				}
				tData = (ThreadData*)calloc(1,sizeof(ThreadData));
				tData->context = context;
				tData->fnPtr = handleIcapData;
				tData->client_socket = 0;
				pthread_create(&tReceiver,NULL,receiver,tData);
				sleep(3);
				ret = icapSendData(context, httpResponseBody1, strlen(httpResponseBody1));
				printf("Chunk1 Sent\n");
				if(ret < 0)
				{
								printf("Error [%d] [%s] in icapSendData\n", ret,icapGetErrorString(ret));
								exit(1);
				}
				sleep(3);
				ret = icapSendData(context, httpResponseBody2, strlen(httpResponseBody2));
				printf("Chunk2 Sent\n");
				if(ret < 0)
				{
								printf("Error [%d] [%s] in icapSendData\n", ret,icapGetErrorString(ret));
								exit(1);
				}
				sleep(3);
				ret = icapSendData(context, httpResponseBody3, strlen(httpResponseBody3));
				printf("Chunk3 Sent\n");
				if(ret < 0)
				{
								printf("Error [%d] [%s] icapSendData", ret,icapGetErrorString(ret));
								exit(1);
				}
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
int formChunkedData(char *buf, int bufLen, char **chunkedData, int *chunkLen)
{
  int offset = 0;
  if(chunkedData == NULL || chunkLen == NULL)
  {
    return ICAP_ERR_INVALID_PARAMS;
  }
  if(bufLen > 0xFFFF)
  {
    return ICAP_PROTO_ERR_CHUNK_SIZE_EXCEEDED;
  }
  if(buf == NULL || bufLen < 1)
  {
    *chunkedData = strdup(ICAP_CHUNK_END);
    *chunkLen = strlen(ICAP_CHUNK_END);
    return 0;
  }
  *chunkedData = (char*)calloc(4/*chunkSize*/ + 2/*\r\n*/ + bufLen + 2/*\r\n*/ + 4, sizeof(char));
  offset += sprintf(*chunkedData + offset, "%x\r\n", bufLen);
  memcpy(*chunkedData + offset, buf, bufLen);
  offset += bufLen;
  offset += sprintf(*chunkedData + offset, "\r\n");
  *chunkLen = offset;
  return 0;
}

