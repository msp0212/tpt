#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "hiredis.h"
#include "async.h"
#include "libev.h"

char gRedisIP[16] = {0,};
int gRedisPort;
redisAsyncContext *gAsyncContext = NULL;

int connectToRedis(char *redisIP, int redisPort);
void mySignalHandler(int signo);

void connectCallback(const redisAsyncContext *c, int status);
void disconnectCallback(const redisAsyncContext *c, int status);
void commandCallback(redisAsyncContext *c, void *r, void *privdata);

int main(int argc, char **argv)
{
	signal(SIGINT,mySignalHandler);
	int ret = 0 ;
	char cmd[64] = {0,};
	if(argc != 3)
	{
		fprintf(stderr,"%s: Wrong No. of arguments to program!!!\nUsage : %s <RedisIP>  <RedisPort>\n", __FUNCTION__, argv[0]);
		exit(1);
	}
	strncpy(gRedisIP, argv[1], sizeof(gRedisIP));
	gRedisPort = atoi(argv[2]);
	ret = connectToRedis(gRedisIP, gRedisPort);
	if(ret != 0)
	{
		fprintf(stderr,"%s : Error in redisConnect !!!\n", __FUNCTION__);
	}
	redisAsyncSetDisconnectCallback(gAsyncContext,disconnectCallback);
	snprintf(cmd, sizeof(cmd),"PSUBSCRIBE __key*__:*");
	redisAsyncCommand(gAsyncContext, commandCallback, NULL, cmd);
	ev_loop(EV_DEFAULT_ 0);
	return 0;
}

int connectToRedis(char *redisIP, int redisPort)
{
	if(redisIP == NULL || redisPort < 0 || redisPort > 65535)
	{
		fprintf(stderr, "%s : Invalid params received !!!\n", __FUNCTION__);
		return -1;
	}
	gAsyncContext = redisAsyncConnect(redisIP, redisPort);
	if(gAsyncContext->err)
	{
		fprintf(stderr,"%s : Error [%s] in redisAsyncConnect !!!\n", __FUNCTION__, gAsyncContext->errstr);
		return -1;
	}
	redisLibevAttach(EV_DEFAULT_ gAsyncContext);
	redisAsyncSetConnectCallback(gAsyncContext,connectCallback);
	return 0;
}

void mySignalHandler(int signo)
{
  printf("Recieved SIGINT (kill -%d)\n",signo);
  redisAsyncDisconnect(gAsyncContext);
  printf("Exiting now...\n");
  exit(0);
}

void connectCallback(const redisAsyncContext *c, int status)
{
	if(status != REDIS_OK)
	{
		fprintf(stderr,"%s : Error [%s]\n", __FUNCTION__, c->errstr);
		return;
	}
	printf("%s : Connected successfully to Redis Server [%s : %d]\n", __FUNCTION__, gRedisIP, gRedisPort);
	return ;
}

void disconnectCallback(const redisAsyncContext *c, int status)
{
    if (status != REDIS_OK) {
        printf("Error: %s\n", c->errstr);
        return;
    }
    printf("Disconnected...\n");
}

void commandCallback(redisAsyncContext *c, void *r, void *privdata)
{
	int i = 0;
	redisReply *reply = r;
	if(reply == NULL)
	{
		fprintf(stderr,"%s : Error [%s]\n", __FUNCTION__, c->errstr);
		return;
	}
	else if(reply->type == REDIS_REPLY_ERROR)
	{
		fprintf(stderr,"%s : Error [%s]\n", __FUNCTION__, reply->str);
	}
	else if(reply->type == REDIS_REPLY_ARRAY)
	{
		for(i=0; i<reply->elements; i++)
		{
			printf("[%s]\n",reply->element[i]->str);
		}
	}
	else
	{
		fprintf(stderr,"%s : Unexpected reply type !!!\n", __FUNCTION__);
	}
	return;
}
