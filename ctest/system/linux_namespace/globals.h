#ifndef GLOBALS_H
#define GLOBALS_H

#define MB (1024 * 1024)
#define CHILD_STACK_SIZE (1 * MB)

extern char g_child_stack[CHILD_STACK_SIZE];
extern char g_cmd[512];
extern int g_netcnt;
extern int g_vethid;

#endif

