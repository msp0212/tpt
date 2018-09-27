#include<stdio.h>
#include<stdlib.h>
#include<error.h>
#include<string.h>
#include<sys/types.h>
#include<sys/un.h>
#include<sys/unistd.h>
#include<sys/socket.h>

int main()
{
	int cliSock;
	int retVal;
	int len;
	char buf[128]={0,};
	struct sockaddr_un remote;

	cliSock = socket(AF_UNIX,SOCK_STREAM,0);
	if(cliSock < 0)
	{
		perror("socket");
		exit(1);
	}

	remote.sun_family = AF_UNIX;
	strcpy(remote.sun_path,"myUnixSocket");
	len = strlen(remote.sun_path) + sizeof(remote.sun_family);

	printf("Trying to connect...\n");

	retVal = connect(cliSock, (struct sockaddr*)&remote, len);
	if(retVal < 0)
	{
		perror("connect");
		exit(1);
	}

	while(1)
	{
		printf(">");

		if(fgets(buf, sizeof(buf),stdin) == NULL)
		{
			perror("fgets");
			break;
		}
		
		len = send(cliSock, buf, strlen(buf),0);
		if(len < 0)
		{
			perror("send");
			break;
		}
		
		if(strncasecmp(buf,"bye",3)==0)
		{
			break;
		}
		
		len = recv(cliSock,buf,sizeof(buf),0);
		if(len < 0)
		{
			perror("recv");
			break;
		}
		printf("Server says : %s",buf);
	}	

	close(cliSock);
}
