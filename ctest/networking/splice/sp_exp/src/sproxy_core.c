/*System Headers*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<poll.h>
/*Local Headers*/
#include<sproxy_core.h>
#include<sproxy_globals.h>
#include<sproxy_net.h>
#include<sproxy_splice.h>
#include<sproxy_log.h>

static int proxy_poll_and_tunnel(int client_socket, int os_socket);
static int proxy_recv_and_send(int recv_sock, int send_sock);

void tunnel_the_request(void *arg)
{
	struct Proxy_Info *proxy_info = NULL;	
	int ret = 0;
	
	if (arg == NULL) {
		log_msg(LOG_CRIT, "NULL params received !!!");
	}
	proxy_info = (struct Proxy_Info *)arg;
	log_msg(LOG_INFO, "Client IP [%s] Client Port [%d]", inet_ntoa(proxy_info->client_address.sin_addr), proxy_info->client_address.sin_port);
	
	ret = tcp_connect_to_server(g_dest_ip, g_dest_port, &proxy_info->os_socket);
	if (ret != 0) {
		log_msg(LOG_ERR, "Error in tcp_connect_to_server !!!");
		goto label1 ;
	}
	ret = set_socket_non_blocking(&proxy_info->os_socket);
	if(ret < 0) {
		log_msg(LOG_ALERT, "Error in set_socket_non_blocking !!!", errno, strerror(errno));
		goto label;
	}
	log_msg(LOG_DEBUG, "Waiting for data...");
	ret = proxy_poll_and_tunnel(proxy_info->client_socket, proxy_info->os_socket);
	if (ret < 0) {
		log_msg(LOG_ERR, "Error in proxy_poll_and_tunnel !!!\n");
		goto label;
	}
	else if (ret == 0) {
		goto label;
	}
label1:
	close(proxy_info->client_socket);
	free(proxy_info); proxy_info = NULL;
	return;
label:
	close(proxy_info->client_socket);
	close(proxy_info->os_socket);
	free(proxy_info); proxy_info = NULL;
	return;
}

static int proxy_poll_and_tunnel(int client_socket, int os_socket)
{
	struct pollfd fd_array[2];
	int recv_sock = 0, send_sock = 0;
	int pipefd_arr[2];
	int ret = 0;

	fd_array[0].fd = client_socket;
	fd_array[0].events = POLLIN|POLLPRI;
	fd_array[1].fd = os_socket;
	fd_array[1].events = POLLIN|POLLPRI;

	ret = pipe(pipefd_arr); 
	if (ret != 0) { 
		log_msg(LOG_ERR, "Error [%d] [%s] in pipe !!!", errno, strerror(errno)); 
		return -1; 
	}

	while (1) {
		ret = poll(fd_array, 2, SPROXY_POLL_TIMEOUT);
		if (ret < 0) {
			log_msg(LOG_ERR, "Error [%d] [%s] in poll !!!", errno, strerror(errno));
			ret = -1;
 			goto label;
		}
		else if (ret == 0) {
			log_msg(LOG_WARNING, "Poll timed out while waiting for data!!!");
			ret = -1;
			goto label;
		}
		else {
			if (fd_array[0].revents&POLLIN || fd_array[0].revents&POLLPRI) {
				log_msg(LOG_DEBUG, "Data available on client socket");
				recv_sock = client_socket;
				send_sock = os_socket;
				ret = proxy_recv_and_send(recv_sock, send_sock);
				if (ret < 0) {				
					log_msg(LOG_CRIT, "Error in proxy_recv_and_send !!!");
					ret = -1;
					goto label;
				}
				else if (ret == 0) {
					log_msg(LOG_INFO, "Close from one side");
					goto label;
				}

			}
			else if (fd_array[1].revents&POLLIN || fd_array[1].revents&POLLPRI) {
				log_msg(LOG_DEBUG, "Data available on os socket");
				ret = make_splice(os_socket, client_socket, pipefd_arr, g_splice_size);
				if (ret < 0) {
					log_msg(LOG_ERR, "Error in make_splice !!!");
					ret = -1;
					goto label;
				}
				else if (ret == 0) {
					goto label;
				}
			}
		}
	}
label:
	close(pipefd_arr[0]);
	close(pipefd_arr[1]);
	return ret;
}

static int proxy_recv_and_send(int recv_sock, int send_sock)
{
	int ret = 0;
	char buf[MAX_LENGTH] = {0,};
	int buf_len;
	while (1) {
		ret = recv(recv_sock, buf, MAX_LENGTH, 0);
		if (ret < 0) {
			if (errno == EAGAIN) {
				ret = 1;
				goto label;
			}
			else {
				log_msg(LOG_ERR, "Error [%d] [%s] in recv !!!", errno, strerror(errno));
				ret = -1;
			}
			goto label;
		}
		else if (ret == 0) {
			log_msg(LOG_INFO, "Peer performed an orderly shutdown");
			ret = 0;
			goto label;
		}
		else {
			log_msg(LOG_DEBUG, "Received [%d] bytes", ret);
		}
		buf_len = ret;
		ret = tcp_send_data_with_poll(send_sock, buf, buf_len, 0);
		if (ret < 0) {
			log_msg(LOG_ERR, "Error in tcp_send_data_with_poll !!!");
			goto label;
		}
		else {
			log_msg(LOG_DEBUG, "Sent [%d] bytes", ret)
		}
	}
label:
	return ret;
}
