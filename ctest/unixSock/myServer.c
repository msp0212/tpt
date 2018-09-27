#include<stdio.h>
#include<stdlib.h>
#include<error.h>
#include<string.h>
#include<sys/types.h>
#include<sys/un.h>
#include<sys/socket.h>
#include<pthread.h>

int g_ClientCount;
pthread_mutex_t g_mutex_ClientCount;

void *clientHandler(void *arg)
{
	int *cliSock	;
	int len;
	char buf[128]={0,};

	printf("Total connected clients = %d\n",g_ClientCount);
	cliSock =(int*)arg; 
	while(1)
	{
		len = recv(*cliSock,buf,sizeof(buf),0);
		if(len < 0)
		{
			perror("recv");
			break;
		}

		if(strncasecmp(buf,"bye",3) == 0)
		{
			break;
		}
		
		len = send(*cliSock,&buf,len,0);
		if(len<0)
		{
			perror("send");
			break;
		}
	}

	close(*cliSock);
	pthread_mutex_lock(&g_mutex_ClientCount);
	g_ClientCount--;
	pthread_mutex_unlock(&g_mutex_ClientCount);
	printf("Total connected clients = %d\n",g_ClientCount);
	return NULL;
}

int main()
{
	int servSock, *cliSock;
	struct sockaddr_un local, remote;
	pthread_t tClientHandler;
	int len;

	cliSock = (int*)calloc(1,sizeof(int));
	servSock = socket(AF_UNIX,SOCK_STREAM,0);
	if(servSock < 0)
	{
		perror("socket");	
		exit(1);
	}
	
	local.sun_family = AF_UNIX;
	strcpy(local.sun_path,"myUnixSocket");
	unlink(local.sun_path);
	len = strlen(local.sun_path) + sizeof(local.sun_family);

	if( bind(servSock, (struct sockaddr*)&local ,len) == -1)
	{
		perror("bind");
		exit(1);
	}
	
	if(listen(servSock, 1) == -1)
	{
		perror("listen");
		exit(1);
	}
	
	while(1)
	{
		len = sizeof(remote);
		*cliSock = accept(servSock, (struct sockaddr*)&remote, &len);
		printf("DATA = %s\n",remote.sun_path);
		pthread_create(&tClientHandler, NULL, clientHandler, (void*)cliSock);
		pthread_mutex_lock(&g_mutex_ClientCount);
		g_ClientCount++;
		pthread_mutex_unlock(&g_mutex_ClientCount);
	}

	return 0;
}	
