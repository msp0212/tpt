#include<string.h>
/*Local Headers*/
#include<sproxy_globals.h>
#include<workq.h>

int g_server_socket;

WorkQT tunnel_workq;

char g_dest_ip[16] = {0, };
int g_dest_port;
int g_splice_size = 0;
