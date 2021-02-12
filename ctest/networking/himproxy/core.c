/*
 * This file contains proxy core parts
 */
/*System Headers*/
#include <stdlib.h>
#include <errno.h>
#include <string.h>
//#include<sys/types.h>          
#define _GNU_SOURCE
#include <sys/socket.h>
//#include<netinet/in.h>
#include <arpa/inet.h>
/*Local Headers*/
#include <core.h>
#include <signal_handler.h>
#include <globals.h>
#include <log.h>
#include <poller.h>
#include <network.h>

static struct poller_info *poll_info = NULL;
static struct poller_context proxy_ctx = NULL;

static void start_proxy(void);
static int init_proxy(void); 
static int handle_epoll_events(struct epoll_event *evs, int ev_count);
static int handle_client_event(struct epoll_event *ev);
static int handle_client_data(struct proxy_info *pinfo);
static int recv_client_data(struct proxy_info *pinfo);
static int handle_proxy_listen_event(struct epoll_event *ev);
static int accept_and_add_conn_to_poller(void);

void create_multiple_child(int num_child)
{
	int i = 0;
	int count = 0;

	if (num_child <= 0) {
		num_child = 1;
	} else if (num_child > MAX_CHILD) {
		num_child = MAX_CHILD;
	}
	for(i = 0; i < num_child;  i++) {
		if(start_child(i)) {
			count++;
			g_active_child_count++;
		}

	}
	if(!count)
		log_msg(LOG_EMERG, "Failed to start any child processes !!!")
	else 
		log_msg(LOG_INFO, "Proxy started...");
	return;
}

int start_child(int index)
{
	pid_t pid;

	pid = fork();			
	if (pid < 0)/*error*/ {
		log_msg(LOG_ALERT, "Error [%d] [%s] in fork !!!", 
					errno, strerror(errno));
		g_child_table[index].pid = -1;
	}
	else if (pid == 0)/*child process*/ {
		start_proxy();
	} else if (pid > 0)/*parent process*/ {
		log_msg(LOG_INFO, "Created Child Process with pid [%d]",
					pid);
		g_child_table[index].pid = pid;
	}
	return pid;
}

int free_proxy_info(struct proxy_info **proxy_info)
{
	int ret = 0;

	if (proxy_info == NULL) {
		log_msg(LOG_CRIT, "NULL params received");
		ret = -1;
		goto exit;
	}
	free(*proxy_info); *proxy_info = NULL;
exit :
	return ret;
}

int proxy_parent_cleanup()
{

	log_msg(LOG_DEBUG, "Proxy parent cleanup...");
	close(g_server_sock);
	close_system_logger();
	log_msg(LOG_DEBUG, "parent cleanup done");
	return 0;
}

int proxy_child_cleanup()
{
	log_msg(LOG_DEBUG, "Proxy child cleanup...");
	close(g_server_sock);
	close_system_logger();
	log_msg(LOG_DEBUG, "child cleanup done");
	return 0;
}

static void start_proxy(void)
{
	int ret = 0;
	
	if ((ret = init_proxy())) {
		log_msg(LOG_EMERG, "Error in init_proxy. ret [%d]", ret);
		goto exit;
	}

	log_msg(LOG_INFO, "Waiting for client connections...");	
	/*Event Loop*/
	while(1) {
		ret = epoll_wait(poll_info->epoll_fd, poll_info->events, 
						poll_info->event_size, -1);
		if (ret < 0) {
			if (errno == EINTR) {
				continue;
			} else {
				log_msg(LOG_ALERT, 
					"Error [%d] [%s] in epoll_wait",
						errno, strerror(errno));
			}
		} else {
			log_msg(LOG_DEBUG, "%d sockets ready...\n", ret);
			if ((ret = handle_epoll_events(poll_info->events,
								ret)) < 0) {
				log_msg(LOG_CRIT,
					"Error %d in handle_epoll_events",
									ret);
			}
		}
	}
exit :
	_exit(0);
}

static int init_proxy(void) 
{
	int ret = 0;

	register_all_signal_handler(child_signal_handler);
	
	if ((ret = init_poller(&poll_info)) < 0) {
		log_msg(LOG_CRIT, "Error in init_poller. ret [%d]", ret);
		ret = -1;
		goto exit;
	}
	
	proxy_ctx.pside = PROXY_SIDE;
	proxy_ctx.pdata.proxy_serv_sock = g_server_sock;
	if ((ret = add_to_poller(poll_info->epoll_fd, g_server_sock,
						EVENT_IN, &proxy_ctx)) < 0) {
		log_msg(LOG_CRIT, "Error in add_to_poller. ret [%d]", ret);
		ret = -3;
		goto exit;
	}
exit :
	return ret;
}

static int handle_epoll_events(struct epoll_event *evs, int ev_count)
{
	int ret = 0;
	int i = 0;
	struct poller_context *ctx = NULL;

	if (evs == NULL) {
		log_msg(LOG_ERR, "NULL params passed");
		ret = -1;
		goto exit;
	}
	
	for (i = 0; i < ev_count; i++) {
		ctx = evs[i].data.ptr;
		if (ctx->pside == PROXY_SIDE) {
			if ((ret = handle_proxy_listen_event(evs + i)) < 0 ) {
				log_msg(LOG_ERR, 
					"Error %d in "
					"handle_proxy_listen_event", ret);
				ret  = -2;
			}
		} else if (ctx->pside == CLIENT_SIDE) {
			if ((ret = handle_client_event(evs + i)) < 0) {
				log_msg(LOG_ERR, 
					"Error %d in "
					"handle_client_event", ret);
				ret  = -3;
			}

		} else {

		}
	}
exit:
	return ret;
}
static int handle_client_event(struct epoll_event *ev)
{
	int ret = 0;
	struct proxy_info *pinfo = NULL;
	if (ev == NULL) {
		ret= -1;
		log_msg(LOG_ERR, "NULL params passed");
		goto exit;
	}
	pinfo = ev->data.ptr;
	if (ev->events & EPOLLERR) {
		ret = -2;
		log_msg(LOG_ERR, "Error occured on client sock %d", pinfo->poll_sock);
	} else if (ev->events & EPOLLHUP) {
		ret = -3;
		log_msg(LOG_ERR, "Hang up happened on client sock %d", pinfo->poll_sock);
	} else if (ev->events & EPOLLIN || ev->events & EPOLLPRI){
		log_msg(LOG_DEBUG, "Data available on client sock %d", pinfo->poll_sock);
		if ((ret = handle_client_data(pinfo)) < 0) {
			ret = -4;
			log_msg(LOG_ERR, "Error %d in handle_client_data");
		}
	} else {
		log_msg(LOG_WARNING, "Error on client sock %d", pinfo->poll_sock);
		ret = -4;
	}
exit:
	return ret;
}

static int recv_client_data(struct proxy_info *pinfo)
{
	int ret = 0;
						
	if (pinfo == NULL) {
		ret = -1;
		log_msg(LOG_ERR, "NULL params passed");
		goto exit;
	}

	if (pinfo->cinfo->cbuf_size == 0) {
		pinfo->cinfo->cbuf_size = ALLOC_SIZE;
		pinfo->cinfo->cbuf = malloc(pinfo->cinfo->cbuf_size *
						sizeof(*pinfo->cinfo->cbuf));
		if (pinfo->cinfo->cbuf == NULL) {
			log_msg(LOG_CRIT, "Error in malloc");
			ret = -2;
			goto exit;
		}
	}
	while (1) {
		ret = recv(pinfo->cinfo->client_sock, 
				pinfo->cinfo->cbuf + pinfo->cinfo->cbuf_len,
				pinfo->cinfo->cbuf_size - pinfo->cinfo->cbuf_len,
				0);
		if (ret < 0) {
			if (errno == EINTR) {
				continue;
			} else if (errno == EAGAIN) {
				log_msg(LOG_DEBUG, "EAGAIN on client sock %d",
							pinfo->cinfo->client_sock);
				ret = 0;
				break;
			} else {
				log_msg(LOG_ERR, "Error [%d] [%s] in recv",
							errno, strerror(errno));
				ret = -3;
				break;
			}
		} else if (ret == 0) { 
			log_msg(LOG_INFO, "client closed the conn on sock %d",
							pinfo->cinfo->client_sock);
			break;
		} else {
			pinfo->cinfo->cbuf_len += ret;
			log_msg(LOG_DEBUG,"recv_bytes %d, cbuf_len %d", ret, 
							pinfo->cinfo->cbuf_len);
			log_msg(LOG_DEBUG, "cbuf [%.*s]", 
					pinfo->cinfo->cbuf_len,
					pinfo->cinfo->cbuf);
			if (pinfo->cinfo->cbuf_size - pinfo->cinfo->cbuf_len 
							<= REALLOC_THRESHOLD) {
				pinfo->cinfo->cbuf_size *= 2;
				pinfo->cinfo->cbuf = realloc(
							pinfo->cinfo->cbuf, 
							pinfo->cinfo->cbuf_size);
				if (pinfo->cinfo->cbuf == NULL) {
					log_msg(LOG_CRIT, "Error in realloc");
					ret = -3;
				} else {
					log_msg(LOG_DEBUG, 
						"realloced cbuf to %d", 
						pinfo->cinfo->cbuf_size);
				}
			}
		}
	}
exit:
	return ret;
}

static int handle_proxy_listen_event(struct epoll_event *ev)
{
	int ret = 0;

	if (ev == NULL) {
		ret = -1;
		log_msg(LOG_ERR, "NULL params passed");
		goto exit;
	}
	if (ev->events & EPOLLERR) {
		ret = -2;
		log_msg(LOG_ERR, "Error occured on proxy server sock");
	} else if (ev->events & EPOLLHUP) {
		ret = -3;
		log_msg(LOG_ERR, "Hang up happened on proxy server sock");
	} else if (ev->events & EPOLLIN || ev->events & EPOLLPRI){
		if((ret = accept_and_add_conn_to_poller()) < 0) {
			log_msg(LOG_ERR, 
				"Error %d in accept_and_add_conn_to_poller", ret)
			ret = -4;
		}
	} else {
		log_msg(LOG_WARNING, "Error on proxy server sock");
		ret = -5;
	}
exit:
	return ret;
}

static int accept_and_add_conn_to_poller(void)
{
	int ret = 0;
	int client_sock = 0;
	int addr_len = 0;
	struct sockaddr_in client_addr;
	struct proxy_info *pinfo = NULL;
	
	addr_len = sizeof(client_addr);
	while(1) {
		client_sock = accept4(g_server_sock,
					(struct sockaddr *)&client_addr,
						(socklen_t *)&addr_len, 
							SOCK_NONBLOCK);
		if (client_sock < 0) {
			if (errno == EINTR) {
				continue;
			} else if (errno == EAGAIN) {
				log_msg(LOG_DEBUG, "EAGAIN on proxy server sock");
				ret = 0;
				break;
			} else {
				log_msg(LOG_ERR, "Error [%d] [%s] in accept",
							errno, strerror(errno));
				ret = -1;
				break;
			}
		} else {
			log_msg(LOG_DEBUG, "Accepted conn from client [%s:%d]", 
						inet_ntoa(client_addr.sin_addr),
							ntohs( client_addr.sin_port));
		}
	}
	return ret;
}

static int create_proxy_conn_info(struct proxy_info **pinfo)
{
	int ret = 0;

	if (pinfo == NULL) {
		ret = -1;
		log_msg(LOG_ERR, "NULL params passed");
		goto exit;
	}
	*pinfo = calloc(1, sizeof(**pinfo));
	if (*pinfo == NULL) {
		log_msg(LOG_CRIT, "Error in malloc");
		ret = -2;
		goto exit;
	}
	(*pinfo)->cinfo = calloc(1, sizeof(*((*pinfo)->cinfo)));
	if ((*pinfo)->cinfo == NULL) {
		log_msg(LOG_CRIT, "Error in malloc");
		free(pinfo); pinfo = NULL;
		ret = -3;
		goto exit;
	}
exit :
	return ret;
}

static int fill_proxy_conn_info(struct proxy_info *pinfo, int client_sock, 
						struct sockaddr_in client_addr)
{
	int ret = -1;
	if (pinfo == NULL) {
		ret = -1;
		log_msg(LOG_ERR, "NULL params passed");
		goto exit;
	}
	pinfo->poll_sock = client_sock;
	pinfo->cinfo->client_sock = client_sock;
	pinfo->cinfo->client_addr = client_addr;
	if (get_orig_dest_addr(client_sock,
				&pinfo->cinfo->dest_addr,
				&pinfo->cinfo->dest_ip, 
				&pinfo->cinfo->dest_port) < 0) {
		log_msg(LOG_CRIT, "Error in get_orig_dest_addr");
	}
	log_msg(LOG_DEBUG,
			"Dest Addr %s:%d", pinfo->cinfo->dest_ip, 
			pinfo->cinfo->dest_port);


exit:
	return ret;
}
static int add_conn_to_poller(sock)
{
	if ((ret = add_to_poller(poll_info->epoll_fd,
					client_sock,
					EVENT_IN,
					pinfo)) < 0) {
		log_msg(LOG_CRIT, 
				"Error in add_to_poller. ret [%d]"
				, ret);
		ret = 0;
	}

}
