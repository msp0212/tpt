#include<stdio.h>
#include<stdlib.h>
#include "hiredis.h"
#include<string.h>
redisContext *gContext;

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

int redis_sorted_set_revrange(redisContext *context, char *key,
		int with_scores, long int min,
		long int max, char ***arr,
		int *arr_size)
{
	int ret = 0;
	int i = 0;

	redisReply *reply = NULL;

	if (context == NULL || key == NULL ||
			arr == NULL || arr_size == NULL) {
		ret = -1;
		goto exit;
	}
	if (with_scores == 1) {
		reply = redisCommand(context,
				"ZREVRANGE %s %ld %ld withscores",
				key, min, max);
	} else {

		reply = redisCommand(context, "ZREVRANGE %s %ld %ld",
				key, min, max);
	}
	if (reply == NULL) {
		ret = -2;
		goto exit;
	} else if (reply->type == REDIS_REPLY_ERROR) {
		ret = -3;
		goto exit;
	} else if (reply->type == REDIS_REPLY_NIL) {
		ret = -4;
		goto exit;
	} else if (reply->type == REDIS_REPLY_ARRAY) {
		*arr_size = reply->elements;
		*arr = malloc(sizeof(*arr) * (*arr_size));
		for (i = 0; i < *arr_size; i++) {
			(*arr)[i] = malloc(reply->element[i]->len + 1);
			strcpy((*arr)[i], reply->element[i]->str);
		}
	} else {
		//Nothing to do
		ret = -5;
	}
exit:
	if(reply != NULL)
		freeReplyObject(reply);
	return ret;
}

int redis_set_members(redisContext *context , char *key, char ***arr,
		             int *arr_size )
{
	int ret = 0;
	int i =0;
	redisReply *reply = NULL;

	if(context == NULL || key == NULL || arr == NULL ||
			arr_size == NULL  ){
		ret = -1;
		goto exit;
	}
	reply = redisCommand(context, "SMEMBERS %s", key);
	if(reply == NULL){
		ret = -2;
		goto exit;
	} else if(reply->type == REDIS_REPLY_ERROR){
		ret = -3;
		goto exit;
	} else if(reply->type == REDIS_REPLY_NIL){
		ret = -4;
		goto exit;
	} else if (reply->type == REDIS_REPLY_ARRAY) {
		*arr_size = reply->elements;
		*arr = malloc(sizeof(*arr) * (*arr_size));
		for (i = 0; i < *arr_size; i++) {
			(*arr)[i] = malloc(sizeof((*arr)[i]) * reply->element[i]->len + 1);
			strcpy((*arr)[i], reply->element[i]->str);
		}
	} else {
		/*Nothing to do*/
		ret = -5;
	}
exit:
	if(reply != NULL)
		freeReplyObject(reply);
	return ret;

}

int main()
{
	char ip[16] = "172.19.13.25";
	int port = 6379;
	int ret = 0;
	char **arr = NULL;
	int arr_size = 0;
	int i = 0;

	connect2redis(ip,port); 
#if 0
	ret = redis_sorted_set_revrange(gContext, "cache_prq", 0, 0, 20, &arr, &arr_size);

	if (ret < 0) {
		printf("Error in redis_sorted_set_revrange\n");
		return 1;
	}
#endif
	ret = redis_set_members(gContext, "test", &arr, &arr_size);

	if (ret < 0) {
		printf("Error in redis_sorted_set_revrange\n");
		return 1;
	}
	for (i = 0; i < arr_size; i++) {
		printf("[%s]\n", arr[i]);
	}
	redisFree(gContext);
	return 0;

	
}
