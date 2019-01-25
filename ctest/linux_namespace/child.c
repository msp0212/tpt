#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "globals.h"
#include "helper.h"


static inline void child_get_cmd(char *cmd, int len)
{
	int ch;
	int i;

	for (i = 0; (ch = getchar()) != '\n'; i++) {
		cmd[i] = ch;
	}
	cmd[i] = '\0';
}

static void child_cmd_loop()
{
#define EXIT_STR "q"
	while(1) {
		printf(">");
		child_get_cmd(g_cmd, sizeof(g_cmd));
		if (strncmp(g_cmd, EXIT_STR, strlen(EXIT_STR)) == 0) {
			printf("Exiting now...\n");
			break;
		}
		if (strlen(g_cmd) > 0) {
			system(g_cmd);
		}
	}
#undef EXIT_STR
}

int child_fn(void *arg)
{

	sleep(1);
	printf("###Child Process###\n"
		"PID: %ld, PPID: %ld\n", (long)getpid(), (long)getppid());
	exec_cmd("ip link");
	snprintf(g_cmd, sizeof(g_cmd), "ifconfig vethm%x-1 192.168.1.%d/24 up",
							g_vethid, g_netcnt);
	exec_cmd(g_cmd);
	child_cmd_loop();	
	return 0;	
}


