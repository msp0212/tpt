#ifndef PROXY_GLOBALS_H
#define PROXY_GLOBALS_H
#include<proxy_core.h>
#include<proxy_config.h>
#include<proxy_poller.h>
#include<workq.h>

extern int g_server_sock; /*proxy listening socket*/
extern int g_active_child_count;
extern struct child_table g_child_table[MAX_CHILD];
extern struct proxy_config g_proxy_config;
extern WorkQT g_parse_request_workq;
extern WorkQT g_forward_request_workq;
extern WorkQT g_parse_response_workq;
extern struct poller_info g_client_poller_info;
extern struct poller_info g_os_poller_info;
#endif 
