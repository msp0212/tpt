#ifndef SPROXY_NET_H
#define SPROXY_NET_H

typedef int(*fp_handle_data)(char *, int , void *);
int set_socket_non_blocking(int *sock);
int tcp_bind_and_listen(char *bind_ip, int bind_port, int *server_socket);
int tcp_connect_to_server(char *server_ip, int server_port, int *local_socket);
int tcp_send_data( int socket, char *buf, int buf_len, int flags );
int tcp_send_data_with_poll( int socket, char *buf, int buf_len, int flags );
int tcp_receive_data_with_poll(int socket, char *buf, int buf_len, int flags);
int make_splice(int fd_in, int fd_out, int *pipefd_arr, size_t len);
#endif
