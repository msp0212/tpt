#ifndef NETWORK_H
#define NETWORK_H

int set_socket_non_blocking(int *sock);
int tcp_bind_and_listen(char *bind_ip, int bind_port, int *server_sock);
int tcp_connect_to_server(char *server_ip, int server_port, int *local_sock);
int tcp_transparent_connect_to_server(char *server_ip, int server_port, 
					int *local_socket, char *client_ip,
							int client_port);
int set_socket_ip_transparent(int *sock);
int get_orig_dest_addr(int sock, struct sockaddr_in *addr,
				 char **dest_ip, int *dest_port);
#endif
