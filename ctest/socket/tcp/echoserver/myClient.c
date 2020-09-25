#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define IP_ADDRESS "172.19.13.25"
#define PORT    33000
#define MAX_LENGTH 2048

int main()
{
	int sock, ret, i = 0 ;
	struct sockaddr_in servAddr;
	char buf[MAX_LENGTH] = {0,};

	/*Get the socket*/
		sock = socket(AF_INET,SOCK_STREAM,0);
	if(sock < 0)
    {
        fprintf(stderr,"%s: Socket Creation Error [%d] [%s]\n", __FUNCTION__, errno, strerror(errno));
        return -1;
    }
    /*Set server address*/
    servAddr.sin_family = AF_INET;
    servAddr.sin_port   = htons(PORT);
    if(inet_aton(IP_ADDRESS, &(servAddr.sin_addr)) == 0)
    {
        fprintf(stderr,"%s : Error in inet_aton  !!!\n", __FUNCTION__);
        return -1;
    }
	/*Connect to server*/
	ret = connect(sock, (struct sockaddr *)&servAddr, sizeof(servAddr));
	if(ret < 0)
	{
		fprintf(stderr,"%s: Connect Error [%d] [%s]\n", __FUNCTION__, errno, strerror(errno));
        return -1;
	}
	printf("Connected to IP [%s] Port [%d]\n",IP_ADDRESS,PORT);
	while(1)	
	{
		if(fgets(buf, MAX_LENGTH, stdin) == NULL)
		{
			fprintf(stderr,"%s: fgets returned NULL\n",__FUNCTION__);
			break;
		}
		/*send the buffer*/
		ret = send(sock , buf, strlen(buf)+1, 0);
		if(ret < 0)
		{
			fprintf(stderr,"%s: send Error [%d] [%s]\n", __FUNCTION__, errno, strerror(errno));
			break;
		}
		/*receive back the buffer*/
		ret = recv(sock , &buf, MAX_LENGTH, 0);
		if(ret == 0)/*Perform a orderly shutdown*/
		{
			printf("Closing the connection\n");
			break;
		}
		else if(ret < 0)
		{
			fprintf(stderr,"%s: recv Error [%d] [%s]\n", __FUNCTION__, errno, strerror(errno));
            break;
		}
		printf("Server echoed [%s]\n",buf);
	}
	close(sock);
	return 0;
}
