#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<errno.h>
#include<time.h>
#include"hiredis.h"

/*Globals*/
char gRedisIP[16];
int gRedisPort;
redisContext *gRedisContext = NULL;
/*function declarations*/
int connect2redis(char *ip, int port);
int getKeys(redisContext *context, char *pattern, char ***keys, int *noOfKeys);
int getKey(redisContext *context, char *key, char **value);
int setKey(redisContext *context, char *key, char *value);

int main(int argc , char **argv)
{
	int ret;
	char *value = NULL;
	#define main_cleanup() { \
	redisFree(gRedisContext); \
	}
	if(argc < 2)
	{
		fprintf(stderr,"%s : Wrong no of arguments to the program\nUsage : %s <redis IP> <redis Port> \n",__FUNCTION__, argv[0]);
		exit(1);
	}
	strncpy(gRedisIP,argv[1],sizeof(gRedisIP));
	gRedisPort = atoi(argv[2]);
	/*Open the log file*/
	fprintf(stderr,"%s : gRedisIP [%s] gRedisPort [%d]\n",__FUNCTION__, gRedisIP, gRedisPort);	
	/*connect to redis*/
	ret = connect2redis(gRedisIP,gRedisPort);	
	if(ret < 0)
	{
		exit(1);
	}
	while(1)
	{
		ret = getKey(gRedisContext, "foo",&value);
		if(ret < 0)
		{
			sleep(10);
		}
		else
		{
			printf("foo -> %s\n",value);
		}
		free(value);value = NULL;
		sleep(10);
	}
	main_cleanup();
	return 0;
}

int connect2redis(char *ip,int port)
{
	 if(ip == NULL || port < 0)
	 {
	 	fprintf(stderr,"%s : NULL params received !!!\n", __FUNCTION__);
		return -1;
	 }
	 gRedisContext=redisConnect(ip,port);
   if(gRedisContext->err)
   {
    fprintf(stderr,"%s : Error [%d] [%s] connecting to redis\n",__FUNCTION__, gRedisContext->err, gRedisContext->errstr);
    return -1;
   }
   else
   {
    fprintf(stderr,"%s : Connected successfully to Redis at %s:%d\n",__FUNCTION__, ip, port);
		return 0;
   }
}

int getKey(redisContext *context, char *key, char **value)
{
  redisReply *reply;
  int iRet = -1;
  if( context == NULL || key == NULL || value == NULL)
  {
         fprintf(stderr, "%s: NULL params received !!!\n",__FUNCTION__);
         return -1;
  }
  reply = redisCommand(context,"GET %s", key);
  if(reply == NULL)
  {
          fprintf(stderr,"%s: ERROR(reply null) [%d] [%s] in getting  REDIS key\n",__FUNCTION__, context->err, context->errstr);
          iRet = -1;
          goto label;
  }
  else if(reply->type == REDIS_REPLY_ERROR)
  {
          fprintf(stderr,"%s: ERROR(reply err) [%s] in getting  REDIS key\n", __FUNCTION__, reply->str);
          iRet = -1;
          goto label;
  }
  else if(reply->type == REDIS_REPLY_NIL)
  {
         fprintf(stderr,"%s: ERROR(reply nil) in getting REDIS key\n",__FUNCTION__);
         iRet = -1;
         goto label;
  }
  else if(reply->type == REDIS_REPLY_STRING)
  {
          *value = (char*)calloc(reply->len + 1, sizeof(char));
          strcpy(*value, reply->str);
          iRet = 0;
  }
   label:
        if(reply!= NULL)
  freeReplyObject(reply);
  return iRet;
}

