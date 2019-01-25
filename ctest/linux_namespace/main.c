#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>

#include "globals.h"
#include "parent.h"

int main()
{
	int child_pid;

	if ((child_pid = parent_spawn_child()) < 0) {
		perror("parent_spawn_child");
		exit(1);
	}
	parent_init_veth_pair(child_pid);	
	waitpid(child_pid, NULL, 0);
	return 0;
}
