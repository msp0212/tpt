#ifndef PROXY_NET_H
#define PROXY_NET_H

int set_socket_non_blocking(int *sock);
int tcp_bind_and_listen(char *bind_ip, int bind_port, int *server_sock);
int tcp_connect_to_server(char *server_ip, int server_port, int *local_sock);
int tcp_transparent_connect_to_server(char *server_ip, int server_port, int *local_socket, char *client_ip, int client_port);
#endif
