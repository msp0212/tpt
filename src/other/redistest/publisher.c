#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "hiredis.h"
#include "async.h"
#include "libev.h"

void getCallback(redisAsyncContext *c, void *r, void *privdata)
{
		int i;
		redisReply *reply = r;
		if(reply == NULL)
		{
				printf("ERROR : %s\n",c->errstr);
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
}

void connectCallback(const redisAsyncContext *c, int status) {
    if (status != REDIS_OK) {
        printf("Error: %s\n", c->errstr);
        return;
    }
    printf("Connected...\n");
}

void disconnectCallback(const redisAsyncContext *c, int status) {
    if (status != REDIS_OK) {
        printf("Error: %s\n", c->errstr);
        return;
    }
    printf("Disconnected...\n");
}


int main (int argc, char **argv) 
{
    signal(SIGPIPE, SIG_IGN);

	int i = 0 ;
    redisAsyncContext *c = redisAsyncConnect("172.19.3.25", 6900);
    
    if (c->err)
    {
        printf("Error: %s\n", c->errstr);
        return 1;
    }

    redisLibevAttach(EV_DEFAULT_ c);
    redisAsyncSetConnectCallback(c,connectCallback);
    redisAsyncSetDisconnectCallback(c,disconnectCallback);
	
	redisAsyncCommand(c, getCallback, NULL, "PUBLISH redis HELLO");

	//Just testing
/*	for(i=0;i<100;i++)
		redisAsyncCommand(c, getCallback, NULL, "PUBLISH CHNL hello_%d",i); */
	
	redisAsyncDisconnect(c);
    ev_loop(EV_DEFAULT_ 0);
    return 0;
}
