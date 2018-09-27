#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<assert.h>
#include<sys/types.h>
#include<sys/wait.h>

int main(int argc, char **argv)
{
	int pipefd[2];
	int ret = 0;
	pid_t pid;
	char buf;

	if (argc != 2) {
		printf("Error in passing arguments to the program\n"
				"Usage : %s user_string\n", argv[0]);
		exit(1);
	}

	ret = pipe(pipefd);
	if (ret < 0) {
		perror("Error in pipe()");
		exit(1);
	}
	
	pid = fork();
	if (pid < 0) {
		perror("Error in fork()");
		exit(1);
	} else if (pid == 0) {
		printf("Child Process...\n");
		close(pipefd[1]);
		while (read(pipefd[0], &buf, 1) != 0) {
			write(1, &buf, 1);
		}
		write(1, "\n", 1);
		close(pipefd[0]);
		exit(0);
	} else {
		printf("Parent Process\n");
		close(pipefd[0]);
		write(pipefd[1], argv[1], strlen(argv[1]));
		close(pipefd[1]);
		wait(NULL);
		exit(0);
	}
	return 0;
}
