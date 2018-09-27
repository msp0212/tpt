#include<stdio.h>
#include<stdlib.h>
#include "hiredis.h"
#include<string.h>
redisContext *gContext;
redisReply *gReply;
typedef struct my
{
	char *name;
	int vol;
}MY;

void connect2redis(char ip[],int port)
{
	gContext=redisConnect(ip,port);
	 if(gContext->err)
	 {
	 	printf("ERROR : %s \n",gContext->errstr);
	 	exit(1);
	 }
	 else
	 {
	 	printf("Connected successfully to redis at %s:%d\n",ip,port);
	 }

}

void setstruct(void *ptr, int size)
{
		gReply = redisCommand(gContext,"set h_mohit %b",ptr,size);
		if(gReply==NULL)
		{
				printf("ERROR : %s\n",gContext->errstr);
				exit(1);
		}
		else if(gReply->type == REDIS_REPLY_ERROR)
		{
				printf("ERROR error: %s\n",gReply->str);
		}
		else
		{
			printf("struct set!!!\n");
		}
}

void getstruct(void **ptr)
{
		int len;
		gReply = redisCommand(gContext,"get h_mohit");
		if(gReply==NULL)
		{
				printf("ERROR : %s\n",gContext->errstr);
				exit(1);
		}
		else if(gReply->type == REDIS_REPLY_ERROR)
		{
				printf("ERROR error: %s\n",gReply->str);
		}
		else if(gReply->type == REDIS_REPLY_STRING)
		{
				len = gReply->len;
				printf("REPLY string of lenght %d : %s\n",len,gReply->str);
				*ptr = (void*)calloc(len,sizeof(char));
				memcpy(*ptr, gReply->str, len);
		}
}

int main()
{
	char ip[16] = "172.19.10.31";
	int port = 6905;
	char *key="pushserver";
	char *value="singh";
	MY *m1 = NULL, *m2 = NULL;
	void *pData = NULL;
	
	connect2redis(ip,port);
	m1 = (MY*)calloc(1,sizeof(MY));
	m1->name = (char*)calloc(16,sizeof(char));
	strcpy(m1->name,"mohit");
	m1->vol = 1234;
	setstruct(m1,sizeof(MY));
	getstruct(&pData);
	m2 = (MY*)pData;
	printf("devlog : %s %d\n",m2->name,m2->vol);
	free(m1);
	free(pData);
	freeReplyObject(gReply);

	redisFree(gContext);
	return 0;

	
}
