/*
 *This file contains proxy's globals declarations.
 *Try keeping globals declarations to a minimum for obvious reasons :)
 */
/*Local Headers*/
#include<proxy_globals.h>
/*Common Headers*/

int g_server_sock;
int g_active_child_count = 0;
struct child_table g_child_table[MAX_CHILD];
struct proxy_config g_proxy_config;
WorkQT g_parse_request_workq;
WorkQT g_forward_request_workq;
WorkQT g_parse_response_workq;
struct poller_info g_client_poller_info;
struct poller_info g_os_poller_info;
