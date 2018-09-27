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

void executeCommand(char *mycmd,char ***keys, int *num)
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
		*num = gReply->elements;
		*keys = (char**)calloc(*num,sizeof(char*));
		for(i=0;i<gReply->elements;i++)
		{
		//	printf("%s ",gReply->element[i]->str);
			(*keys)[i] = (char*)calloc(gReply->element[i]->len + 1, sizeof(char));
			strcpy((*keys)[i],gReply->element[i]->str);
		}
		printf("\n");
	}


}

int main()
{
	char ip[16] = "172.19.10.31";
	int port = 6905;
	char mycmd[32];
//	char *key="pushserver";
	char *value="singh";
	char **key = NULL;
	int num;
	//connect to redis
	connect2redis(ip,port);
	int i =0;

	//form the command
	sprintf(mycmd,"keys *");
	//execute command
	executeCommand(mycmd,&key,&num);
	freeReplyObject(gReply);
	for(i=0;i<num;i++)
	{
		printf("%s \n",key[i]);
	}

	for(i=0;i<num;i++)
	{
		free(key[i]);key[i]=NULL;
	}
	free(key);
	redisFree(gContext);
	return 0;

}
