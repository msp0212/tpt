#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sched.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include "globals.h"
#include "helper.h"
#include "child.h"

#define NETWORK_COUNT_FILE "/var/.exec_ns_net_cnt"

static inline unsigned int parent_gen_vethid()
{
	return (unsigned int)time(NULL);
}

static int parent_get_netcnt()
{
	/* This function does synchronize the access to the critical section
	 * viz. NETWORK_COUNT_FILE */
	int fd;
	int ret;
	unsigned int cnt;
	
	fd = open(NETWORK_COUNT_FILE, O_RDWR | O_CREAT);
	if (fd < 0) {
		perror("read");
		goto err;
	}

	ret = read(fd, &cnt, sizeof(cnt));
	if (ret < 0) {
		perror("read");
		goto err;
	} else {
		if (ret < sizeof(cnt)) {
			ret = cnt = 2;
		} else {
			ret = cnt++;
		}
	}
	lseek(fd, 0L, SEEK_SET);
	write(fd, &cnt, sizeof(cnt));
	close(fd);
	return ret;
err:
	return -1;
}

int parent_spawn_child()
{
	int child_pid;

	/*clone a child with separate PID and NET namespaces*/
	if ((child_pid = clone(child_fn, g_child_stack + CHILD_STACK_SIZE,
			CLONE_NEWPID | CLONE_NEWNET | CLONE_VM | SIGCHLD,
					NULL)) < 0) {
		perror("clone");
		goto err;
	}
	printf("###Parent Process###\n"
		"PID: %ld, Child PID: %d\n", (long)getpid(), child_pid);
	return child_pid;
err:
	return -1;
}

void parent_init_veth_pair(int child_pid)
{
	/*add veth pair from parent namespace to child namespace*/	
	g_vethid = parent_gen_vethid();
	snprintf(g_cmd, sizeof(g_cmd), "ip link add name vethm%x-0 "
			"type veth peer name vethm%x-1 netns %d", 
				g_vethid, g_vethid, child_pid);
	exec_cmd(g_cmd);
	g_netcnt = parent_get_netcnt();
	
	/*configure ip address in parent namespace*/
	/*snprintf(g_cmd, sizeof(g_cmd), "ifconfig vethm%x-0 10.%d.1.1/24 up",
							g_vethid, g_netcnt);*/
	//exec_cmd(g_cmd);

}

