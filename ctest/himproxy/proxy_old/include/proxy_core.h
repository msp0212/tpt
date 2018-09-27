#ifndef PROXY_CORE_H
#define PROXY_CORE_H
/*System Headers*/
#include<unistd.h>
#include<netinet/in.h>
/*Local Headers*/
#include<proxy_request_handler.h>
#include<proxy_response_handler.h>
/*Common headers*/
#include<http_utils.h>

#define MAX_CHILD 16

struct child_table {
	pid_t pid;
};

struct proxy_info {
	/*client section*/
	int client_sock; 
	struct sockaddr_in client_addr;
	char *client_buf;
	int client_buf_size;
	int client_buf_curr_len;
	int client_buf_parsed_len;
	struct proxy_req_info *curr_req;
	struct proxy_req_info *last_req;

	/*server section*/
	int os_sock;
	char *os_buf;
	int os_buf_size;
	int os_buf_curr_len;
	int os_buf_parsed_len;
	struct proxy_resp_info *curr_resp;
	struct proxy_resp_info *last_resp;
};

void create_multiple_child(int num_child);
int start_child(int index);
int free_proxy_info(struct proxy_info **proxy_info);
int proxy_parent_cleanup();
int proxy_child_cleanup();
#endif
