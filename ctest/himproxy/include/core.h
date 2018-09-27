#ifndef PROXY_CORE_H
#define PROXY_CORE_H
/*System Headers*/
#include<unistd.h>
#include<netinet/in.h>

#define MAX_CHILD 16
#define ALLOC_SIZE 8192
#define REALLOC_THRESHOLD 128

struct child_table {
	pid_t pid;
};

struct proxy_info;

struct client_info {
	
	int client_sock; 
	struct sockaddr_in client_addr;
	char *cbuf;
	int cbuf_size;
	int cbuf_len;
	
	struct proxy_info *pinfo;
};

struct server_info {
	
	int os_sock;
	struct sockaddr_in dest_addr;
	char *dest_ip;
	int dest_port;

	char *sbuf;
	int sbuf_size;
	int sbuf_len;
	
	struct proxy_info *pinfo;
};

union poller_data {
	struct client_info *cinfo;
	struct server_info *sinfo;
	int proxy_serv_sock;
};

enum poll_side {
	PROXY_SIDE,
	CLIENT_SIDE,
	SERVER_SIDE
};

struct poller_context {
	enum poll_side pside;
	union poller_data pdata;
};

struct proxy_info {
	struct client_info *cinfo;
	struct server_info *sinfo;
};

void create_multiple_child(int num_child);
int start_child(int index);
int free_proxy_info(struct proxy_info **proxy_info);
int proxy_parent_cleanup();
int proxy_child_cleanup();
#endif
