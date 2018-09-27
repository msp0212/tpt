/*
 * This file contains proxy core parts
 */
/*System Headers*/
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>          
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
/*Local Headers*/
#include<proxy_core.h>
#include<proxy_globals.h>
#include<proxy_log.h>
#include<proxy_signal_handler.h>
#include<proxy_net.h>
#include<proxy_request_handler.h>
#include<proxy_request_forwarder.h>
#include<proxy_response_handler.h>
#include<proxy_poller.h>
/*Common Headers*/
#include<workq.h>

static void start_proxy(int index);
static int add_to_request_poller(int client_sock,
			struct sockaddr_in client_addr);
static int init_workq(void);
static int intialize_proxy(void);

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
		start_proxy(index);
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
	int tmp = 0;

	if (proxy_info == NULL) {
		log_msg(LOG_CRIT, "NULL params received");
		ret = -1;
		goto exit;
	}
	free((*proxy_info)->client_buf);(*proxy_info)->client_buf = NULL;
	tmp = free_proxy_req_list(&(*proxy_info)->curr_req);
	if (tmp < 0) {
		ret |= tmp;
		log_msg(LOG_CRIT, "Error in free_proxy_req_list");
	}
	free(*proxy_info); *proxy_info = NULL;
exit :
	return ret;
}

int proxy_parent_cleanup()
{

	log_msg(LOG_DEBUG, "Proxy parent cleanup...");
	if (free_config() < 0) {
		log_msg(LOG_ERR, "Error in free_config");
	}
	close(g_server_sock);
	close_system_logger();
	log_msg(LOG_DEBUG, "parent cleanup done");
	return 0;
}
int proxy_child_cleanup()
{
	log_msg(LOG_DEBUG, "Proxy child cleanup...");
	close(g_server_sock);
	destroy_poller(&g_client_poller_info);
	destroy_poller(&g_os_poller_info);
	close_system_logger();
	log_msg(LOG_DEBUG, "child cleanup done");
	return 0;
}
static void start_proxy(int index)
{
	int client_sock = 0;
	int addr_len = 0;
	struct sockaddr_in client_addr;
		
	if (register_all_signal_handler(child_signal_handler) < 0) {
		log_msg(LOG_EMERG, "Error in register_signal_handler !!!");
		goto exit;
	}
	if(intialize_proxy() < 0) {
		log_msg(LOG_EMERG, "Error in intialize_proxy");
		goto exit;
	}
	log_msg(LOG_INFO, "Waiting for client connections...");	
	while (1) {
		addr_len = sizeof(client_addr);
		client_sock = accept(g_server_sock, 
					(struct sockaddr *)&client_addr,
						(socklen_t *)&addr_len);
		if (client_sock < 0) {
			if (errno == EINTR) {
				continue;
			} else {
				log_msg(LOG_ALERT, "Error [%d] [%s] in accept",
						errno, strerror(errno));
				goto exit;
			}
		}
		log_msg(LOG_DEBUG, "Accepted connection from Client [%s : %d]",
				  	      inet_ntoa(client_addr.sin_addr),
							client_addr.sin_port);

		if (add_to_request_poller(client_sock, client_addr) < 0) {
			close(client_sock);
			log_msg(LOG_ALERT, 
				 "Error in add_to_parse_request_workq");
		}
	}
exit :
	_exit(0);
}

static int add_to_request_poller(int client_sock, 
					struct sockaddr_in client_addr)
{
	struct linger linger;
	struct proxy_info *proxy_info = NULL;
	int ret = 0;

	if (set_socket_non_blocking(&client_sock) < 0) {
		log_msg(LOG_ALERT, "Error in set_socket_non_blocking !!!"); 
		ret = -1;
		goto exit;
	}
	linger.l_onoff = 0;
        linger.l_linger = 0;
	setsockopt(client_sock, SOL_SOCKET, SO_LINGER , (char *)&linger, sizeof(linger));
	proxy_info = calloc(1, sizeof(struct proxy_info));
        proxy_info->client_sock = client_sock;
	proxy_info->client_addr = client_addr;
	if (add_to_poller(g_client_poller_info.epoll_fd, client_sock, EVENT_IN, 
						(void*)proxy_info) < 0) {
		log_msg(LOG_ERR, "Error in add_to_poller");
		ret = -1;
		goto cleanup_exit;
	}
	log_msg(LOG_DEBUG, "Added fd [%d] to poller", client_sock);
	goto exit;
cleanup_exit:
	if (free_proxy_info(&proxy_info) < 0) {
		log_msg(LOG_CRIT, "Error in free_proxy_info");
	}
exit:
	return ret;
}

static int intialize_proxy(void)
{
	int ret = 0;

	if (init_workq() < 0) {
		log_msg(LOG_EMERG, "Error in init_workq!!!");
		ret = -1;
		goto exit;
	}
	
	g_client_poller_info.workq = &g_parse_request_workq;
	g_client_poller_info.poller_routine = proxy_default_poller;

	g_os_poller_info.workq = &g_parse_response_workq;
	g_os_poller_info.poller_routine = proxy_default_poller;

	if (init_poller(&g_client_poller_info) < 0 ){ 
		log_msg(LOG_EMERG, "Error in init_poller");
		ret = -1;
		goto exit;
	}
	if (init_poller(&g_os_poller_info) < 0 ){
		log_msg(LOG_EMERG, "Error in init_poller");
		ret = -1;
		goto exit;
	}
exit :
	return ret;
}

static int init_workq(void)
{
	int ret = 0;

	if (WorkQInit(&g_parse_request_workq, g_proxy_config.workq_parallelism,
				g_proxy_config.workq_thread_timeout, 
					g_proxy_config.workq_length,
					receive_parse_request) != 0)
	{
		log_msg(LOG_EMERG, "WorkQInit failed for parse_request_workq");
		ret = -1;
		goto exit;
	}
	if (WorkQInit(&g_forward_request_workq, g_proxy_config.workq_parallelism,
				g_proxy_config.workq_thread_timeout, 
					g_proxy_config.workq_length,
					forward_request) != 0)
	{
		log_msg(LOG_EMERG, "WorkQInit failed for forward_request_workq");
		ret = -1;
		goto exit;
	}
	if (WorkQInit(&g_parse_response_workq, g_proxy_config.workq_parallelism,
				g_proxy_config.workq_thread_timeout, 
					g_proxy_config.workq_length,
					receive_parse_response) != 0)
	{
		log_msg(LOG_EMERG, "WorkQInit failed for parse_request_workq");
		ret = -1;
		goto exit;
	}
exit :
	return ret;
}
