#define _GNU_SOURCE
#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <sched.h>

#define MB (1024 * 1024)
#define CHILD_STACK_SIZE (1 * MB)

static char child_stack[CHILD_STACK_SIZE];

static inline unsigned int parent_gen_vethid();
static inline void child_get_cmd(char *cmd, int len);
static int child_fn(void *arg);

int main()
{
	int child_pid;
	char cmd[256];
	unsigned int vethid;

	/*clone a child with separate PID and NET namespaces*/
	child_pid = clone(child_fn, child_stack + CHILD_STACK_SIZE,
			CLONE_NEWPID | CLONE_NEWNET | SIGCHLD, NULL);
	if (child_pid < 0) {
		perror("clone");
		exit(1);
	}

	printf("###Parent Process###\n"
		"PID: %ld, Child PID: %d\n", (long)getpid(), child_pid);

	/*add veth pair from parent namespace to child namespace*/	
	vethid = parent_gen_vethid();
	snprintf(cmd, sizeof(cmd), "ip link add name vethm%x-0 "
			"type veth peer name vethm%x-1 netns %d", 
				vethid, vethid, child_pid);
	printf("%s\n", cmd);
	system(cmd);

	waitpid(child_pid, NULL, 0);

	return 0;
}

static inline unsigned int parent_gen_vethid()
{
	return (unsigned int)time(NULL);
}

static int child_fn(void *arg)
{
#define EXIT_STR "bye"
	char cmd[512];
	
	sleep(1);
	printf("###Child Process###\n"
		"PID: %ld, PPID: %ld\n", (long)getpid(), (long)getppid());
	system("ip link");
	printf("\n");
	
	while(1) {
		printf(">");
		child_get_cmd(cmd, sizeof(cmd));
		if (strncmp(cmd, EXIT_STR, strlen(EXIT_STR)) == 0) {
			printf("Exiting now...\n");
			break;
		}
		if (strlen(cmd) > 0) {
			system(cmd);
		}
	}
	return 0;
#undef EXIT_STR
}

static inline void child_get_cmd(char *cmd, int len)
{
	int ch;
	int i;

	for (i = 0; (ch = getchar()) != '\n'; i++) {
		cmd[i] = ch;
	}
	cmd[i] = '\0';
}
