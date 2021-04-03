#ifndef SPROXY_GLOBALS_H
#define SPROXY_GLOBALS_H

#define MAX_LENGTH 8192

#define SPROXY_POLL_TIMEOUT 10000

#include<netinet/in.h>
#include<workq.h>

struct Proxy_Info
{
	int client_socket;
	struct sockaddr_in client_address;
	int os_socket;
};

extern WorkQT tunnel_workq;

extern int g_server_socket;

extern char g_dest_ip[16];
extern int g_dest_port; 
extern int g_splice_size; 
#endif
