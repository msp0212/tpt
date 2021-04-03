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
	char ip[16] = "172.19.10.31";
	int port = 6905;
	char mycmd[256];
	//connect to redis
	connect2redis(ip,port);
	int i =0;
	char **keys = NULL;
	char **values = NULL;
	keys = (char**)calloc(2,sizeof(char*));
	values = (char**)calloc(2,sizeof(char*));
	for(i=0;i<2;i++)
	{
		keys[i] = (char*)calloc(8,sizeof(char));
		values[i] = (char*)calloc(8,sizeof(char));
		sprintf(keys[i],"k%d",i);
		sprintf(values[i],"v%d",i);
	}
	char **retValues = NULL;
	setHash(keys,values,2);
	getHash(keys,&retValues,2);
	for(i=0;i<2;i++)
	{
		printf("rv = %s\n",retValues[i]);
	}

label:
	redisFree(gContext);
	return 0;

}

void setHash(char **keys,char **values, int num)
{		
		int i = 0;
		char test[16] = {0,};
		for(i=0;i<num;i++)
		{
				gReply = redisCommand(gContext,"hset h1 %s %s",keys[i],test);
				freeReplyObject(gReply);
		}
	return;
}

void getHash(char **keys,char ***values, int num)
{
		int i = 0;
		*values = (char**)calloc(num,sizeof(char*));	
		for(i=0;i<num;i++)
		{
				gReply = redisCommand(gContext,"hget h1 %s",keys[i]);
				(*values)[i] = (char*)calloc(gReply->len + 1, sizeof(char));
				printf("len = %d\n",gReply->len);
				strcpy((*values)[i],gReply->str);
				freeReplyObject(gReply);
		}
	return;
}
