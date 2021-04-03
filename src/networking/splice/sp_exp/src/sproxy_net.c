/*system headers*/
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<poll.h>
/*Local headers*/
#include<sproxy_net.h>
#include<sproxy_globals.h>
#include<sproxy_log.h>

int set_socket_non_blocking(int *sock)
{
	int socket_flags;
	int ret;

	socket_flags = fcntl(*sock, F_GETFL, 0);
	if (socket_flags < 0) {
		log_msg(LOG_ERR, "fcntl Error [%d] [%s] when F_GETFL", errno, strerror(errno));
		return -1;
	}
	socket_flags = socket_flags|O_NONBLOCK;
	ret = fcntl(*sock, F_SETFL, socket_flags);
	if (ret < 0) {
		log_msg(LOG_ERR, "fcntl Error [%d] [%s] when F_SETFL", errno, strerror(errno));
		return -1;
	}
	return 0;
}

int tcp_bind_and_listen(char *bind_ip, int bind_port, int *server_socket)
{
	int n = 1, ret = 0;
	struct sockaddr_in server_address;

	if (bind_ip == NULL || server_socket == NULL || bind_port < 0 || bind_port > 65535) {
		log_msg(LOG_CRIT, "Invalid params received !!!");
		return -1;
	}

	*server_socket = socket(AF_INET,SOCK_STREAM,0);
	if (server_socket < 0) {
		log_msg(LOG_ERR, "Socket Creation Error [%d] [%s] ", errno, strerror(errno));
		return -1;
	}

	setsockopt(*server_socket, SOL_SOCKET, SO_REUSEADDR, (void*)&n ,(socklen_t)sizeof(n));

	server_address.sin_family = AF_INET;
	server_address.sin_port   = htons(bind_port);
	if (inet_aton(bind_ip, &(server_address.sin_addr)) == 0) {
		log_msg(LOG_ERR, "Error in inet_aton  !!! " );
		return -1;		
	}

	ret = bind(*server_socket, (struct sockaddr *)&server_address, sizeof(server_address));
	if (ret < 0) {
		log_msg(LOG_ERR, "Bind Error [%d] [%s] ", errno, strerror(errno));	
		return -1;
	}
	ret = listen(*server_socket, 128);
	if (ret < 0) {
		log_msg(LOG_ERR, "Listen Error [%d] [%s] ", errno, strerror(errno));
		return -1;
	}
	return 0;
}

int tcp_connect_to_server(char *server_ip, int server_port, int *local_socket)
{
	int sock,ret;
	struct sockaddr_in server_address;

	if(server_ip == NULL || server_port < 0 || server_port > 65535 || local_socket == NULL) {
		log_msg(LOG_ERR, "Invalid params received !!! " );
		return -1;
	}

	sock = socket(AF_INET,SOCK_STREAM,0);
	if (sock < 0) {
		log_msg(LOG_ERR, "Socket Creation Error [%d] [%s] ", errno, strerror(errno));
		return -1;
	}

	server_address.sin_family = AF_INET;
	server_address.sin_port   = htons(server_port);
	if (inet_aton(server_ip, &(server_address.sin_addr)) == 0) {
		log_msg(LOG_ERR, "Error in inet_aton  !!! ");
		return -1;
	}

	ret = connect(sock, (struct sockaddr *)&server_address, sizeof(server_address));
	if (ret < 0) {
		log_msg(LOG_ERR, "Error [%d] [%s] in connect !!! ", errno, strerror(errno));
		return -1;
	}
	*local_socket = sock;
	return 0;
}

int tcp_send_data(int socket, char *buf, int buf_len, int flags)
{
	int ret = 0, bytes_sent = 0;
	if( buf == NULL || buf_len < 0 || socket < 0 ) {
		log_msg(LOG_ERR, "Invalid params received !!! " );
		return -1;
	}

	while(bytes_sent < buf_len) {
		ret = send( socket, buf + bytes_sent, buf_len - bytes_sent, flags ) ;
		if(ret < 0) {
			if(errno == EINTR)/*Interrupt was caught*/ {
				continue;
			}
			else {
				log_msg(LOG_ERR, "Send Error [%d] [%s] ", errno, strerror(errno));
				return -1;
			}
		}
		bytes_sent += ret;
	}
	return bytes_sent;
}

int tcp_send_data_with_poll(int socket, char *buf, int buf_len, int flags)
{
	struct pollfd fd_array[1];
	int bytes_sent = 0;
	int ret = 0;

	fd_array[0].fd = socket;
	fd_array[0].events = POLLOUT;

	while (bytes_sent < buf_len) {
		ret = poll(fd_array, 1, SPROXY_POLL_TIMEOUT );
		if (ret < 0) {
			if (errno == EINTR){ 
				continue;
			}
			else {
				log_msg(LOG_ERR, "Error [%d] [%s] in poll !!!", errno, strerror(errno));
				return -1;
			}
		}
		else if( ret == 0 )/*timed out and no descriptors were ready*/
		{
			log_msg(LOG_ERR, "Poll timed out !!!")
				return -1;
		}
		else if (ret > 0 && (fd_array[0].revents & POLLOUT)) { /*write will not block now*/
			ret = send(socket, buf + bytes_sent, buf_len - bytes_sent, flags);
			if(ret < 0 ) {
				if(errno == EINTR) {/*Interrupt was caught*/
					continue;
				}
				else {
					log_msg(LOG_ERR, "Send Error [%d] [%s] ", errno, strerror(errno));
					return -1;
				}
			}
			bytes_sent += ret;
		}
	}
	return bytes_sent;
}

int tcp_receive_data_with_poll(int socket, char *buf, int buf_len, int flags)
{	
	int ret = 0; 
	struct pollfd fd_array[1];

	if( buf == NULL || buf_len <= 0) {
		log_msg(LOG_ERR, "Invalid params received !!! ");
		return -1;
	}

	fd_array[0].fd = socket;
	fd_array[0].events = POLLIN|POLLPRI;

	while (1) {
		ret = poll(fd_array, 1, SPROXY_POLL_TIMEOUT);
		if (ret < 0) {
			if (errno == EINTR){
				continue;
			}
			else {
				log_msg(LOG_ERR, "Error [%d] [%s] in poll !!!", errno, strerror(errno));
				return -1;
			}
		}
		else if( ret == 0 )/*timed out and no descriptors were ready*/
		{
			log_msg(LOG_ERR, "Poll timed out !!!")
				return -1;
		}
		else if (ret > 0 && (fd_array[0].revents&POLLIN || fd_array[0].revents&POLLPRI)) { /*recv will not block now*/
			ret = recv(socket, buf, buf_len, flags);
			if(ret < 0 ) {
				if(errno == EINTR) {/*Interrupt was caught*/
					continue;
				}
				else {
					log_msg(LOG_ERR, "Recv Error [%d] [%s] ", errno, strerror(errno));
					return -1;
				}
			}
			else if( ret == 0) {
				log_msg(LOG_DEBUG, "Peer performned a orderly shutdown ");
				return 0;
			}
			return ret;
		}
	}
}

