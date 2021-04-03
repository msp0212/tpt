#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#define IP_ADDRESS "127.0.0.1"
#define PORT	19000
#define MAX_LENGTH 1024

void *Handler(void *arg);
enum buffer_type {
	SEND_BUFFER,
	RECV_BUFFER,
};
typedef struct _handler_data
{
	int cliSock;
	struct sockaddr_in cliAddr;
}HandlerData;

static int set_socket_buffer_size(int sock, enum buffer_type buf_type, int size);
static int get_socket_buffer_size(int sock, enum buffer_type buf_type, int *size);


void handleSignal(int signo)
{
	if(signo == SIGKILL)
    printf("Recieved SIGKILL...Exiting now\n");
    exit(0);
}

int main()
{
	signal(SIGINT,handleSignal);
	int servSock, cliSock ,ret, len ,n ;
	struct sockaddr_in servAddr, cliAddr;
	struct in_pktinfo pki;
	pthread_t tid;
	HandlerData *handler_data;
	int size = 0;

	/*Get the socket*/
	servSock = socket(AF_INET,SOCK_STREAM,0);
	if(servSock < 0)
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

	/*Bind to the address*/
	ret = bind(servSock, (struct sockaddr *)&servAddr, sizeof(servAddr));
	if(ret < 0)
	{
		fprintf(stderr,"%s: Bind Error [%d] [%s]\n", __FUNCTION__, errno, strerror(errno));	
		return -1;
	}
	/*Listen on the socket*/
	ret = listen(servSock, 1);
	if(ret < 0)
	{
		fprintf(stderr,"%s: Listen Error [%d] [%s]\n", __FUNCTION__, errno, strerror(errno));
		return -1;
	}

	/*Start accpeting connections from clients*/
	while(1)
	{
		len = sizeof(cliAddr); 
		printf("Waiting for new connections...\n");
		cliSock = accept(servSock, (struct sockaddr *)&cliAddr, &len);
		if (set_socket_buffer_size(cliSock, RECV_BUFFER, 1) < 0) {
			printf("Error %d %s in set_socket_buffer_size\n",
					errno, strerror(errno));
		}
		if (get_socket_buffer_size(cliSock, RECV_BUFFER, &size) < 0) {
			printf("Error %d %s in get_socket_buffer_size\n",
					errno, strerror(errno));
		}
		printf("socket send buffer size = %d\n", size);
		if(cliSock < 0)
		{
			fprintf(stderr,"%s: Accept Error [%d] [%s]\n", __FUNCTION__, errno, strerror(errno));
			return -1;
		}

		/*original destination address */
		n = sizeof(struct in_pktinfo);
		ret = getsockopt (cliSock, IPPROTO_IP, IP_PKTINFO, &pki, &n);
		if (ret != 0)
		{
			fprintf (stderr, "error getting original destination address. err (#%d %s)\n", errno, strerror(errno));
			close (cliSock);
			return -1;
		}
		printf ("original destination address %s\n", inet_ntoa(pki.ipi_addr));

		/*fill handler data*/
		handler_data = (HandlerData*)calloc(1,sizeof(HandlerData));
		handler_data->cliSock = cliSock;
		memcpy(&(handler_data->cliAddr), &cliAddr, sizeof(handler_data->cliAddr));
		/*create handler thread*/
		pthread_create(&tid, NULL, Handler, (void*)handler_data);
	}
	return 0;	
}	

void *Handler(void *arg)
{
	char *cliIP = NULL;
	char buf[MAX_LENGTH]= {0,};
	HandlerData *handler_data = (HandlerData*)arg;
	int bytes = 0;	

	/*print client details*/
	cliIP = inet_ntoa(handler_data->cliAddr.sin_addr);
	printf("Thread [%lu] Client IP [%s]\nClient Port [%d]\n",pthread_self(), cliIP, handler_data->cliAddr.sin_port);
	while(1)
	{
		printf("Thread [%lu] Waiting for data from client...\n",pthread_self());
		/*receive data from client*/
		bytes = recv(handler_data->cliSock, &buf, MAX_LENGTH,0);
		if(bytes == 0)/*Perform a orderly shutdown*/
		{
			break;
		}
		else if(bytes < 0)
		{
			fprintf(stderr,"%s: Thread [%lu] Recv Error [%d] [%s]\n",
					__FUNCTION__,pthread_self(), errno, strerror(errno));		
			break;
		}
		printf("Thread [%lu] Received %d bytes  from client [%s]\n", pthread_self() ,bytes, buf);
		if(strncasecmp(buf,"bye",3) == 0)
		{
			printf("Thread [%lu] Closing connection with Client IP [%s] port [%d]\n",
					pthread_self(),cliIP, handler_data->cliAddr.sin_port);
			break;
		}
		/*send back the data to client*/
		bytes = send(handler_data->cliSock, buf, bytes, 0);
		if(bytes < 0)
		{
			fprintf(stderr,"%s: Thread [%lu] send Error [%d] [%s]\n", __FUNCTION__,pthread_self(), errno, strerror(errno));
			break;
		}
		printf("Thread [%lu] Sent %d bytes to client [%s]\n", pthread_self() ,bytes, buf);
	}
	/*close the client socket*/
	close(handler_data->cliSock);
	free(handler_data); handler_data = NULL;
	pthread_exit(NULL);
	return ;	
}

static int set_socket_buffer_size(int sock, enum buffer_type buf_type, int size)
{
	int ret = 0;
	int optlen = 0;

	optlen = sizeof(size);
	if(buf_type == SEND_BUFFER) {
		ret = setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &size,
				(socklen_t)optlen);
	} else if (buf_type == RECV_BUFFER) {
		ret = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &size,
				(socklen_t)optlen);
	}
	return ret;
}

static int get_socket_buffer_size(int sock, enum buffer_type buf_type, int *size)
{
	int ret = 0;
	int optlen = 0;

	optlen = sizeof(*size);
	if(buf_type == SEND_BUFFER) {
		ret = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, size,
				(socklen_t *)&optlen);
	} else if (buf_type == RECV_BUFFER) {
		ret = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, size,
				(socklen_t *)&optlen);
	}
	return ret;
}
