/*Local Headers*/
#include <globals.h>
#include <log.h>

int g_log_level = LOG_ERR;
int g_server_sock;
int g_num_child;
int g_active_child_count;
struct child_table g_child_table[MAX_CHILD];
