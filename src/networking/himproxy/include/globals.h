#ifndef GLOBALS_H
#define GLOBALS_H
/*Local Headers*/
#include <core.h>
extern int g_log_level;
extern int g_server_sock;
extern int g_num_child;
extern int g_active_child_count;
extern struct child_table g_child_table[MAX_CHILD];
#endif
