/*
 * This file contains wrappers on the C socket apis
 * */
/*System Headers*/
#include<errno.h>
#include<string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include<fcntl.h>
/*Local Headers*/
#include<proxy_net.h>
#include<proxy_log.h>

int set_socket_non_blocking(int *sock)
{
	int sock_flags;
	int ret;
	if(sock == NULL) {
		log_msg(LOG_CRIT, "Invalid params received !!!");
		return -1;
	}
	sock_flags = fcntl(*sock, F_GETFL, 0);
	if (sock_flags < 0) {
		log_msg(LOG_ERR, "fcntl Error [%d] [%s] when F_GETFL", 
					errno, strerror(errno));
		return -1;
	}
	sock_flags = sock_flags|O_NONBLOCK;
	ret = fcntl(*sock, F_SETFL, sock_flags);
	if (ret < 0) {
		log_msg(LOG_ERR, "fcntl Error [%d] [%s] when F_SETFL",
					errno, strerror(errno));
		return -1;
	}
	return 0;
}

int tcp_bind_and_listen(char *bind_ip, int bind_port, int *server_sock)
{
	int n = 1, ret = 0;
	struct sockaddr_in server_address;

	if (bind_ip == NULL || server_sock == NULL || bind_port < 0 || 
			bind_port > 65535) {
		log_msg(LOG_CRIT, "Invalid params received !!!");
		return -1;
	}
	*server_sock = socket(AF_INET,SOCK_STREAM,0);
	if (server_sock < 0) {
		log_msg(LOG_ERR, "Socket Creation Error [%d] [%s] ", 
					errno, strerror(errno));
		return -1;
	}
	setsockopt(*server_sock, SOL_SOCKET, SO_REUSEADDR, (void*)&n ,
				(socklen_t)sizeof(n));
	server_address.sin_family = AF_INET;
	server_address.sin_port   = htons(bind_port);
	if (inet_aton(bind_ip, &(server_address.sin_addr)) == 0) {
		log_msg(LOG_ERR, "Error in inet_aton  !!! " );
		return -1;
	}
	ret = bind(*server_sock, (struct sockaddr *)&server_address, 
			sizeof(server_address));
	if (ret < 0) {
		log_msg(LOG_ERR, "Bind Error [%d] [%s] ", errno, 
					strerror(errno));
		return -1;
	}
	ret = listen(*server_sock, 128);
	if (ret < 0) {
		log_msg(LOG_ERR, "Listen Error [%d] [%s] ", errno, 
				strerror(errno));
		return -1;
	}
        return 0;
}

int tcp_connect_to_server(char *server_ip, int server_port, int *local_sock)
{
	int ret = 0;
	struct sockaddr_in server_address;

	if(server_ip == NULL || server_port < 0 || server_port > 65535 || 
			local_sock == NULL) {
		log_msg(LOG_ERR, "Invalid params received !!! " );
		return -1;
	}

	*local_sock = socket(AF_INET,SOCK_STREAM,0);
	if (*local_sock < 0) {
		log_msg(LOG_ERR, "Socket Creation Error [%d] [%s] ", errno,
					strerror(errno));
		return -1;
	}
	server_address.sin_family = AF_INET;
	server_address.sin_port   = htons(server_port);
	if (inet_aton(server_ip, &(server_address.sin_addr)) == 0) {
		log_msg(LOG_ERR, "Error in inet_aton  !!! ");
		return -1;
	}
	ret = connect(*local_sock, (struct sockaddr *)&server_address, 
				sizeof(server_address));
	if (ret < 0) {
		log_msg(LOG_ERR, "Error [%d] [%s] in connect !!! ", errno,
					strerror(errno));
		return -1;
	}
	return 0;
}

int tcp_transparent_connect_to_server(char *server_ip, int server_port, int *local_socket, char *client_ip, int client_port)
{
	int sock,ret;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	int n = 1;
	
	if (server_ip == NULL || server_port < 0 || server_port > 65535 || socket == NULL) {
		log_msg(LOG_ERR, "Invalid params received !!! " );
		return -1;
	}
	sock = socket(AF_INET,SOCK_STREAM,0);
	if (sock < 0) {
		log_msg(LOG_ERR, "Socket Creation Error [%d] [%s] ", errno, strerror(errno));
		return -1;
	}
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void*)&n ,(socklen_t)sizeof(n));
	ret = setsockopt(sock, SOL_IP, IP_TRANSPARENT, &n, sizeof(int));
	if (ret < 0) {
		log_msg(LOG_ERR, "Error [%d] [%s] in setsockopt !!! ", errno, strerror(errno));
		return -1;
	}
	client_address.sin_family = AF_INET;
	if (inet_aton(client_ip, &(client_address.sin_addr)) == 0) {
		log_msg(LOG_ERR, "Error in inet_aton  !!! " );
		return -1;
	}
	ret = bind(sock, (struct sockaddr *)&client_address, sizeof(client_address));
	if (ret < 0) {
		log_msg(LOG_ERR, "Error [%d] [%s] in bind for Client [%s : %d]!!! ", errno, strerror(errno), client_ip, client_port);
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
