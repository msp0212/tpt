#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <arpa/inet.h>


#define BUF_SIZE 4
int tcp_connect_to_server(char *server_ip, int server_port, int *local_sock);
int tcp_send_data (int socket, char *buf, int buf_len, int flags );
int main()
{
	int tot_size = BUF_SIZE;
	int offset = 0;
	int bytes_read = 0;
	char *buf = NULL;
	int sock = 0;

	if ( tcp_connect_to_server("172.19.13.25", 9404, &sock) < 0) {
		printf("Error in tcp_connect_to_server\n");
		exit(1);
	}
	buf = (char *)calloc(tot_size, sizeof(char));
	while ((bytes_read = read(0, buf+offset, tot_size - offset)) > 0) {
		if (tcp_send_data(sock, buf+offset, bytes_read, 0) < 0) {
			printf("Error in tcp_send_data\n");
		}
		else {
			printf ("sending [%.*s]\n", bytes_read, buf+offset);
		}
		/*offset += bytes_read;
		if ((tot_size - offset) <= 256)	{
			tot_size = 2*tot_size;
			buf = (char*)realloc(buf, tot_size);
		}*/
	}
	sleep(15);
//	printf("\nbuffer :\n%s", buf);
	free(buf);buf=NULL;
	return 0;
}

int tcp_connect_to_server(char *server_ip, int server_port, int *local_sock)
{
	int ret = 0;
	struct sockaddr_in server_address;

	if(server_ip == NULL || server_port < 0 || server_port > 65535 ||
			local_sock == NULL) {
		printf( "Invalid params received !!! \n" );
		return -1;
	}

	*local_sock = socket(AF_INET,SOCK_STREAM,0);
	if (*local_sock < 0) {
		printf( "Socket Creation Error [%d] [%s] \n", errno,
				strerror(errno));
		return -1;
	}
	server_address.sin_family = AF_INET;
	server_address.sin_port   = htons(server_port);
	if (inet_aton(server_ip, &(server_address.sin_addr)) == 0) {
		printf( "Error in inet_aton  !!! \n");
		return -1;
	}
	ret = connect(*local_sock, (struct sockaddr *)&server_address,
			sizeof(server_address));
	if (ret < 0) {
		printf( "Error [%d] [%s] in connect !!! \n", errno,
				strerror(errno));
		return -1;
	}
	return 0;
}

int tcp_send_data (int socket, char *buf, int buf_len, int flags )
{
	int ret = 0, bytes_sent = 0;
	/*sanity checks*/
	if( buf == NULL || buf_len < 0 || socket < 0 )
	{
		printf( "Invalid params received !!! \n" );
		return -1;
	}
	while(bytes_sent < buf_len)
	{
		ret = send ( socket, buf + bytes_sent, buf_len - bytes_sent, flags ) ;
		if(ret < 0)
		{
			if(errno == EINTR)/*Interrupt was caught*/
			{
				continue;
			}
			else
			{
				printf( "send Error [%d] [%s] \n", errno, strerror(errno));
				return -1;

			}
		}
		bytes_sent += ret;
	}
	return 0;
}
