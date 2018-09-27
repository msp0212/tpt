#include<stdio.h>
#include<stdlib.h>
#include "hiredis.h"
#include "async.h"
#include "libev.h"

redisAsyncContext *gAsyncContext;

/* callback functions */
void connectCallback(const redisAsyncContext *c, int status)
{
		if (status != REDIS_OK) {
				printf("Error: %s\n", gAsyncContext->errstr);
				return;
		}
		printf("Connected...\n");
}

void commandCallback(redisAsyncContext *c, void *r, void *privdata)
{
	int i;
	redisReply *reply = r;
	printf("Inside commandCallback...\n");
	if(reply == NULL)
	{
		printf("ERROR : %s\n",gAsyncContext->errstr);
		exit(1);
	}
	else if(reply->type == REDIS_REPLY_STRING)
	{
		printf("REPLY : %s\n",reply->str);
	}
	else if(reply->type == REDIS_REPLY_INTEGER)
	{
		printf("REPLY : %lld\n",reply->integer);
	}
	else if(reply->type == REDIS_REPLY_STATUS)
	{
		printf("REPLY : %s\n",reply->str);
	}
	else if(reply->type == REDIS_REPLY_ERROR)
	{
		printf("ERROR : %s\n",reply->str);
	}
	else if(reply->type == REDIS_REPLY_NIL)
	{
		 printf("REPLY : nil\n");
	}
	else if(reply->type == REDIS_REPLY_ARRAY)
	{
		printf("REPLY : ARRAY\n");
		for(i=0;i<reply->elements;i++)
		{
			printf("%s ",reply->element[i]->str);
		}
		printf("\n");
	}

	printf("Returning from commandCallback...\n");

}

void disconnectCallback(const redisAsyncContext *c, int status)
{
		if (status != REDIS_OK) {
				printf("Error: %s\n", c->errstr);
				return;
		}
		printf("Disconnected...\n");
}
/*End of callback functions*/


void connect2redis(char ip[],int port)
{
	printf("Inside connect2redis...\n");
	gAsyncContext=redisAsyncConnect((char*)ip,port);
	 if(gAsyncContext->err)
	 {
	 	printf("ERROR : %s \n",gAsyncContext->errstr);
	 	exit(1);
	 }
	 redisLibevAttach(EV_DEFAULT_ gAsyncContext);
	 redisAsyncSetConnectCallback(gAsyncContext,connectCallback); 	
	 printf("Returning from connect2redis...\n");
}

void executeCommand(char *mycmd)
{
	int i;
	printf("Inside executeCommand...\n");
	i=redisAsyncCommand(gAsyncContext,commandCallback,NULL,mycmd);
	printf("Returning from executeCommand [%d]...\n",i);
}

void mySignalHandler(int signo)
{
	printf("Recieved SIGINT (kill -%d)\n",signo);
	printf("Disconnecting from redis...\n");
	redisAsyncDisconnect(gAsyncContext);
	printf("Exiting now...\n");
	exit(0);
}

int main()
{
	signal(SIGINT,mySignalHandler);
	
	//declarations
	char ip[16] = "172.19.10.31";
	int port = 6905;
	char mycmd[32];

	printf("Inside Main...\n");
	
	//connect to redis
	connect2redis(ip,port);
	redisAsyncSetDisconnectCallback(gAsyncContext,disconnectCallback);

	//form the command
	sprintf(mycmd,"SUBSCRIBE redis");
	//	sprintf(mycmd,"get mohit");

	//execute command
	executeCommand(mycmd);

	
	//disconnect from redis
	redisAsyncDisconnect(gAsyncContext);
	

	printf("Wait...\n");
	ev_loop(EV_DEFAULT_ 0);
	printf("Returning from Main...\n");
	return 0;
}
