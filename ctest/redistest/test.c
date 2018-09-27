#include<stdio.h>
#include<stdlib.h>
#include "hiredis.h"
#include<string.h>
#include<pthread.h>
redisContext *gContext;
redisReply *gReply;

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

void executeCommand(char *mycmd,char *s)
{
	gReply = redisCommand(gContext,mycmd);
	int i,len;
	if(gReply==NULL)
	{
		printf("ERROR%s : %s\n",s,gContext->errstr);
		exit(1);
	}
	else if(gReply->type == REDIS_REPLY_STRING)
	{
		len = strlen(gReply->str);
		printf("REPLY%s string of lenght %d : %s\n",s,len,gReply->str);
	}
	else if(gReply->type == REDIS_REPLY_INTEGER)
	{
		printf("REPLY%s integer : %lld\n",s,gReply->integer);
	}
	else if(gReply->type == REDIS_REPLY_STATUS)
	{
		printf("REPLY status: %s\n",gReply->str);
	}
	else if(gReply->type == REDIS_REPLY_ERROR)
	{
		printf("ERROR error: %s\n",gReply->str);
	}
	else if(gReply->type == REDIS_REPLY_NIL)
	{
		 printf("REPLY : nil\n");
	}
	else if(gReply->type == REDIS_REPLY_ARRAY)
	{
		printf("REPLY%s : ARRAY\n",s);
		for(i=0;i<gReply->elements;i++)
		{
			printf("%s ",gReply->element[i]->str);
		}
		printf("\n");
	}


}
void *func1()
{
	char mycmd[256];
	while(1)
	{
		sprintf(mycmd,"get mohit");
		executeCommand(mycmd,"1");
		sleep(1);
	}
}


void *func2()
{
	char mycmd[256];
    while(1)
    {
        sprintf(mycmd,"keys *");
        executeCommand(mycmd,"2");
        sleep(1);
    }

}
int main()
{
	pthread_t t1,t2;
	char ip[16] = "172.19.10.31";
	int port = 6905;
	char mycmd[256];
	//connect to redis
	connect2redis(ip,port);
	pthread_create(&t1,NULL,func1,NULL);
	pthread_create(&t2,NULL,func2,NULL);
	//executeCommand(mycmd);
	//freeReplyObject(gReply);
	pthread_exit(NULL);
	redisFree(gContext);
	return 0;

	
}


