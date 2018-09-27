/*
 * This file contains proxy request handling part*/
/*System Headers*/
#include<errno.h>
#include<string.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
/*Local Headers*/
#include<proxy_request_handler.h>
#include<proxy_log.h>
#include<proxy_core.h>
#include<proxy_globals.h>
/*Common Headers*/
#include<http_utils.h>
#include<workq.h>

#define REQUEST_BUF_SIZE 8192
#define REALLOC_LIMIT 128

static int alloc_client_buf(struct proxy_info *proxy_info, int *rem_len);
static int recv_client_data(struct proxy_info *proxy_info);
static int free_proxy_req_info(struct proxy_req_info **node);
static int parse_request_buffer(struct proxy_info *proxy_info);
static int handle_request(struct proxy_info *proxy_info,
			struct http_request_info *http_request_info);
static int proxy_request_list_add(struct proxy_req_info **head,
			    	  struct proxy_req_info **last,
				  char *req_buf, int req_buf_len,
				 struct http_request_info *http_request_info);
static int add_to_request_list(struct proxy_info *proxy_info,
		                  struct http_request_info *http_request_info);
static int proxy_request_list_print(struct proxy_req_info *head);

void receive_parse_request(void *arg)
{
	struct proxy_info *proxy_info = NULL;
	int ret = 0;

	if (arg == NULL) {
		log_msg(LOG_ERR, "NULL params received !!!");
		goto exit;
	}
	proxy_info = arg;
	ret = recv_client_data(proxy_info);
	if (ret < 0) {
		log_msg(LOG_CRIT, "Error in recv_client_data");
		goto cleanup_exit;
	} else if (ret == 0) {
		goto cleanup_exit;
	} else {
		
	}
	log_msg(LOG_DEBUG, "client_buf [%.*s]", proxy_info->client_buf_curr_len,
						proxy_info->client_buf);
	if (parse_request_buffer(proxy_info) < 0) {
		log_msg(LOG_CRIT, "Error in parse_request_buffer");
		goto cleanup_exit;
	}
	proxy_request_list_print(proxy_info->curr_req);
	goto exit;
cleanup_exit:
	close(proxy_info->client_sock);
        if (free_proxy_info(&proxy_info) < 0) {
		log_msg(LOG_CRIT, "Error in free_proxy_info");
	}
exit :
	return;
}

int free_proxy_req_list(struct proxy_req_info **head) 
{
	int ret = 0;
	int rv = 0;
	struct proxy_req_info *tmp = NULL;
	struct proxy_req_info *curr = NULL;
	if (head == NULL) {
		log_msg(LOG_ERR, "NULL params received");
		ret = -1;
		goto exit;
	}
	curr = *head;
	while (curr != NULL) {
		tmp = curr;
		curr = curr->next;
		rv = free_proxy_req_info(&tmp); 
		if (rv < 0) {
			log_msg(LOG_ERR, "Error in free_proxy_req_info");
			ret |= rv;
		}
	}
exit :
	return ret;
}

static int free_proxy_req_info(struct proxy_req_info **node) 
{
	int ret = 0;

	if (node == NULL) {
		log_msg(LOG_ERR, "NULL params received");
		ret = -1;
		goto exit;
	}
	free((*node)->host); (*node)->host = NULL;
	free((*node)->dest_ip); (*node)->host = NULL;
	if ( http_request_info_free(&(*node)->http_request_info) < 0) {
		log_msg(LOG_ERR, "Error [%d] [%s] in free_http_request_info",
					ret, http_get_error_string(ret));
		ret = -1;
	}
	free(*node); *node = NULL;
exit :
	return ret;
}
static int recv_client_data(struct proxy_info *proxy_info) 
{
	int ret = 0;
	int rem_len = 0;

	if (proxy_info == NULL) {
		log_msg(LOG_ERR, "NULL params received");
		ret = -1;
		goto exit;
	}
	while (1) {
		rem_len = proxy_info->client_buf_size - 
				proxy_info->client_buf_curr_len;
		if (alloc_client_buf(proxy_info, &rem_len) < 0) {
			log_msg(LOG_CRIT, "Error in alloc_client_buf");
			ret = -1;
			goto exit;
		}
		ret = recv(proxy_info->client_sock,
				proxy_info->client_buf + 
				proxy_info->client_buf_curr_len,
				rem_len, 0); 
		if (ret < 0) {
			if (errno == EINTR) {
				continue;
			} else if (errno == EAGAIN) {
				log_msg(LOG_DEBUG, "EAGAIN on fd [%d]", 
						proxy_info->client_sock);
				ret = 1;
				goto exit;
			} else {
				log_msg(LOG_ERR, "Error [%d] [%s] in recv",
						errno, strerror(errno));
				goto exit;
			}
		} else if (ret == 0) {
			log_msg(LOG_DEBUG, "Client closed conn on fd [%d]", 
						   proxy_info->client_sock);
			goto exit;
		} else {
			proxy_info->client_buf_curr_len += ret;
		}
	}
exit :
	return ret;
}

static int alloc_client_buf(struct proxy_info *proxy_info, int *rem_len) 
{
	int ret = 0;
	char *tmp = NULL;

	if (proxy_info == NULL) {
		log_msg(LOG_ERR, "NULL params received");
		ret = -1;
		goto exit;
	}
	if (proxy_info->client_buf_size == 0) {
		proxy_info->client_buf_size = REQUEST_BUF_SIZE;
		*rem_len = REQUEST_BUF_SIZE;
	} else if (*rem_len < REALLOC_LIMIT && 
			proxy_info->client_buf > 0) {
			proxy_info->client_buf_size *= 2;
			*rem_len = proxy_info->client_buf_size -
					proxy_info->client_buf_curr_len;
	}
	tmp = realloc(proxy_info->client_buf, proxy_info->client_buf_size);
	if (tmp == NULL) {
		log_msg(LOG_CRIT, "Error in realloc");
		ret = -1;
		goto exit;
	}
	proxy_info->client_buf = tmp;
exit :
	return ret;
}
static int parse_request_buffer(struct proxy_info *proxy_info)
{
	int ret = 0;
	struct http_request_info *http_request_info = NULL;

	if (proxy_info == NULL) {
		ret = -1;
		goto exit;
	}
	while (1) {

		ret = http_parse_request(proxy_info->client_buf + 
				      	   proxy_info->client_buf_parsed_len, 
					proxy_info->client_buf_curr_len - 
					   proxy_info->client_buf_parsed_len,
					&http_request_info);
		if (ret < 0) {
			ret = -1;
			log_msg(LOG_ERR, "Error [%d] [%s] in http_parse_request", 
					ret, http_get_error_string(ret));
		} else if (ret == 0) {
			 /* Request headers received completely*/
			if (handle_request(proxy_info, http_request_info) < 0) {
				log_msg(LOG_ERR, "Error in handle_request");
				ret = -1;
			}
		} else {
			/* Request headers not completely received */
			break;
		}
	}
exit :
	return ret;
}

static int handle_request(struct proxy_info *proxy_info,
				struct http_request_info *http_request_info)
{
	int ret = 0;

	if (proxy_info == NULL || http_request_info == NULL) {
		ret = -1;
		log_msg(LOG_ERR, "NULL params received");
		goto exit;
	}
	/*
	 * TODO POST and PUT request handling
	if (http_request_info->method == HTTP_METHOD_POST ||
			http_request_info->method == HTTP_METHOD_PUT) {
		ret = http_get_header_value(http_request_info, "Content-Length",
						       strlen("Content-Length"), 
						       &values);
		if (ret < 0) {
			log_msg(LOG_ERR, "Error [%d] [%s] in http_get_header_value")
		}
		goto exit;
	}
	*/
	if ( add_to_request_list(proxy_info, http_request_info) < 0) {
		log_msg(LOG_ERR, "Error in add_to_request_list");
		ret = -1;
		goto exit;
	}
	log_msg(LOG_DEBUG, "Added to request list [%.*s]", 
				http_request_info->header_length,
				proxy_info->client_buf + 
					proxy_info->client_buf_parsed_len);
	proxy_info->client_buf_parsed_len += http_request_info->header_length;
exit :
	return ret;
}

static int add_to_request_list(struct proxy_info *proxy_info,
				 struct http_request_info *http_request_info)
{
	int ret = 0;

	if (proxy_info == NULL || http_request_info == NULL) {
		ret = -1;
		log_msg(LOG_ERR, "NULL params received");
		goto exit;
	}
	ret = proxy_request_list_add(&proxy_info->curr_req, 
				&proxy_info->last_req,
				proxy_info->client_buf + 
					proxy_info->client_buf_parsed_len,
				http_request_info->header_length,
				http_request_info);
	if (ret < 0) {
		log_msg(LOG_CRIT, "Error in proxy_request_list_add");
		ret = -1;
		goto exit;
	}
#if 0
	if (WorkQAdd(&g_forward_request_workq, (void *)proxy_info) != 0) {
		log_msg(LOG_ALERT, 
				"Error in WorkQAdd for g_forward_request_workq !!!");
		ret = -1;
	}
#endif
exit :
	return ret;

}
static int proxy_request_list_add(struct proxy_req_info **head,
				  struct proxy_req_info **last,
   				  char *req_buf, int req_buf_len,
				  struct http_request_info *http_request_info)
{
	int ret = 0;
	struct proxy_req_info *tmp = NULL;

	if (head == NULL || last == NULL || req_buf == NULL
					|| http_request_info == NULL) {
		log_msg(LOG_ERR, "NULL params received");
		ret = -1;
		goto exit;
	}
	tmp = calloc(1, sizeof(struct proxy_req_info));
	if (tmp == NULL) {
		log_msg(LOG_ERR, "Error in calloc");
		ret = -1;
		goto exit;
	}
	tmp->req_buf = req_buf;
	tmp->req_buf_len = req_buf_len;
	tmp->http_request_info = http_request_info;
	tmp->next = NULL;
	if (*head == NULL) {
		*head = tmp;
		*last = tmp;
	} else {
		(*last)->next = tmp;
		*last = tmp;
	}
exit :
	return ret;
}

static int proxy_request_list_print(struct proxy_req_info *head)
{
	struct proxy_req_info *tmp = NULL;

	tmp = head;
	while (tmp != NULL) {
		log_msg(LOG_DEBUG, "[%.*s]", tmp->req_buf_len, tmp->req_buf);
		tmp = tmp->next;
	}
	return 0;
}
