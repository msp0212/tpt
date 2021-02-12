#ifndef PROXY_REQUEST_HANDLER_H
#define PROXY_REQUEST_HANDLER_H
/*Common Headers*/
#include<http_utils.h>
struct proxy_req_info {
	char *req_buf;
	int req_buf_len;
	char *host;
	char *dest_ip;
	struct http_request_info *http_request_info;
	struct proxy_req_info *next;
};

void receive_parse_request(void *arg);
int free_proxy_req_list(struct proxy_req_info **head);
#endif
