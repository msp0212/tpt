/*
 * This file contains code for forwarding the request to OS*/
/*System Headers*/
#include<errno.h>
#include<string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
/*Local Headers*/
#include<proxy_request_forwarder.h>
#include<proxy_dns_lookup.h>
#include<proxy_log.h>
#include<proxy_net.h>
#include<proxy_globals.h>
/*Common Headers*/
#include<http_utils.h>

static int get_os_info(struct proxy_req_info *proxy_req_info);

void forward_request(void *arg)
{
	struct proxy_info *proxy_info = NULL;
	struct proxy_req_info *curr_req = NULL;
	char *client_ip = NULL;
	int client_port = 0;
	int ret = 0;

	if (arg == NULL) {
		log_msg(LOG_ERR, "NULL params received");
		goto exit;
	}
	curr_req = proxy_info->curr_req;
	if (get_os_info(curr_req) < 0) {
		log_msg(LOG_ERR, "Error in get_os_info");
		goto exit;
	}
	if (g_proxy_config.ip_transparency) {
		client_ip = inet_ntoa(proxy_info->client_addr.sin_addr);
		client_port = proxy_info->client_addr.sin_port;
		ret = tcp_transparent_connect_to_server(curr_req->dest_ip, 80,
							&proxy_info->os_sock,
							client_ip, client_port);
	} else {
		ret = tcp_connect_to_server(curr_req->dest_ip, 80, 
						&proxy_info->os_sock);
	}
	if (ret < 0) {
		log_msg(LOG_ERR, "Error connecting to OS");
		goto exit;
	}
	//TODO os side part : send request to os
exit :
	return;
}

static int get_os_info(struct proxy_req_info *proxy_req_info) 
{
	int ret = 0;
	struct value_list *values;
	if (proxy_req_info == NULL) {
		log_msg(LOG_ERR, "NULL params received");
		ret = -1;
		goto exit;
	}
	ret = http_get_header_value(proxy_req_info->http_request_info->
								headers_list,
								"Host",
								strlen("Host"),
								&values);
	if (ret < 0) {
		log_msg(LOG_ERR, "Error [%d] [%s] in http_get_header_value",
					ret, http_get_error_string(ret));
		ret = -1;
		goto exit;
	}
	proxy_req_info->host = calloc(values->value_len + 1, sizeof(char));
	if (proxy_req_info->host == NULL) {
		log_msg(LOG_ALERT, "Error in calloc");
		ret = -1;
		goto exit;
	}
	memcpy(proxy_req_info->host, values->value, values->value_len);
	ret = dns_lookup(proxy_req_info->host, "http", &proxy_req_info->dest_ip);
	if (ret < 0) {
		log_msg(LOG_CRIT, "Error in dns_lookup");
		ret = -1;
	}
exit :
	return ret;
}
