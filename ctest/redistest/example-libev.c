#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "hiredis.h"
#include "async.h"
#include "libev.h"

void getCallback(redisAsyncContext *c, void *r, void *privdata) {
    redisReply *reply = r;
    if (reply == NULL) return;
    printf("argv[%s]: %s\n", (char*)privdata, reply->str);

   /* Disconnect after receiving the reply to GET */
    redisAsyncDisconnect(c); 
/*		int i;
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
		redisAsyncDisconnect(c);*/
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

int main (int argc, char **argv) {
    signal(SIGPIPE, SIG_IGN);

    redisAsyncContext *c = redisAsyncConnect("127.0.0.1", 6389);
    if (c->err) {
        /* Let *c leak for now... */
        printf("Error: %s\n", c->errstr);
        return 1;
    }

    redisLibevAttach(EV_DEFAULT_ c);
    redisAsyncSetConnectCallback(c,connectCallback);
    redisAsyncSetDisconnectCallback(c,disconnectCallback);
//    redisAsyncCommand(c, NULL, NULL, "SET key %b", argv[argc-1], strlen(argv[argc-1]));
// 	  redisAsyncCommand(c, getCallback, (char*)"end-1", "GET key");

 	redisAsyncCommand(c, getCallback, NULL, "PUBLISH CHNL hello");
    ev_loop(EV_DEFAULT_ 0);
    return 0;
}
