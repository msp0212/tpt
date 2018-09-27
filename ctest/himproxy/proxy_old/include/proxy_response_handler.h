#ifndef PROXY_RESPONSE_HANDLER_H
#define PROXY_RESPONSE_HANDLER_H
/*Common Headers*/
#include<http_utils.h>

struct proxy_resp_info {
	char *resp_buf;
	int resp_buf_len;
	struct http_response_info *http_response_info;
	struct proxy_resp_info *next;
};

void receive_parse_response(void *arg);

#endif
