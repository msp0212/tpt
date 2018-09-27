#include<stdio.h>
#include<stdlib.h>
#include "hiredis.h"
#include<string.h>
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

void executeCommand(char *mycmd)
{
	gReply = redisCommand(gContext,mycmd);
	int i,len;
	if(gReply==NULL)
	{
		printf("ERROR : %s\n",gContext->errstr);
		exit(1);
	}
	else if(gReply->type == REDIS_REPLY_STRING)
	{
		len = strlen(gReply->str);
		printf("REPLY string of lenght %d : %s\n",len,gReply->str);
	}
	else if(gReply->type == REDIS_REPLY_INTEGER)
	{
		printf("REPLY integer : %lld\n",gReply->integer);
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
		printf("REPLY : ARRAY\n");
		for(i=0;i<gReply->elements;i++)
		{
			printf("%s ",gReply->element[i]->str);
		}
		printf("\n");
	}


}

int main()
{
	char ip[16] = "127.0.0.1";
	int port = 63222;
	char mycmd[32];
	char *key="mohit";
	char *value="singh";
	//connect to redis
	connect2redis(ip,port);
	int i =0;

	//form the command
	for(i =0 ;i < 1000000 ; i++)
	{
		sprintf(mycmd,"sadd mySet  %s%d",value,i);
		//execute command
		executeCommand(mycmd);
		freeReplyObject(gReply);
	}

	redisFree(gContext);
	return 0;

	
}
