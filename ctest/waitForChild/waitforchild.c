#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<signal.h>
#define NUM_CHILD 2
#define ON 1
#define OFF 0

typedef struct 
{
	pid_t pid;
	int state;
}ChildTable;

ChildTable gChildTable[NUM_CHILD];
int gExitFlag = 0;

int waitForChild(int (*fn_ptr)(int));
int restartChild(int index);
void childProcess();
void parentSignalHandler(int signum, siginfo_t *si, void *uContext);
void childSignalHandler(int signum, siginfo_t *si, void *uContext);

int main()
{
	pid_t pid;
	int i, ret;
	struct sigaction sigact;
	/*register parent signal handler*/
	sigact.sa_sigaction = parentSignalHandler;
	sigact.sa_flags = SA_SIGINFO;
	ret = sigaction(SIGINT, &sigact, NULL);
	ret = sigaction(SIGCHLD, &sigact, NULL);
	if(ret < 0)
	{
		fprintf(stderr,"%s : Error in registering parent signal handler !!!\n", __FUNCTION__);
		exit(1);
	}
	/*create child*/
	for(i=0;i<NUM_CHILD;i++)
	{
					pid = fork();
					if(pid < 0)/*error*/
					{
									fprintf(stderr,"%s : Error [%d] [%s] in fork !!!\n", __FUNCTION__, errno, strerror(errno));
									gChildTable[i].pid = 0;
									gChildTable[i].state = OFF;
					}
					else if(pid == 0)/*child process*/
					{
									childProcess();
					}
					else if(pid > 0)/*parent process*/
					{
									printf("%s : Created Child Process with pid [%d]\n",__FUNCTION__, pid);
									gChildTable[i].pid = pid;
                  gChildTable[i].state = ON;
					}
					sleep(2);
	}
	ret = waitForChild(restartChild);/*wait for child*/
	return 0;	
}
int waitForChild(int (*fn_ptr)(int))
{
	pid_t pid;
	int status,i;
	if(gExitFlag == 1)
	{
		return 0;
	}
	while((pid = waitpid(-1, &status, WUNTRACED))!= 0)
	{
		if(gExitFlag)
		{
			printf("DEVLOG : hahaha !!!\n");
			continue;
		}
		if(pid == -1)
		{
			if(errno == EINTR)
			{
				continue;
			}
			else
			{
				break;
			}
		}	
		else
		{
			for(i=0;i<NUM_CHILD;i++)
			{
				if(pid == gChildTable[i].pid && gChildTable[i].state == ON)
				{
					(*fn_ptr)(i);
				}
			}
		}
	}
	return 0;
}

int restartChild(int index)
{
	pid_t pid;
	if(index < 0 || index >= NUM_CHILD)
	{
		fprintf(stderr,"%s : INVALID params received !!!\n", __FUNCTION__);
		return -1;
	}
	printf("%s : Restarting Process at Index [%d]\n", __FUNCTION__, index);
	pid = fork();
	if(pid < 0)
	{
		fprintf(stderr,"%s : Error [%d] [%s] in fork !!!\n", __FUNCTION__, errno, strerror(errno));
		gChildTable[index].pid = 0;
		gChildTable[index].state = OFF;
		return -1;
	}
	else if(pid == 0)
	{
		childProcess();
	}
	else if(pid > 0)
	{
		printf("%s : Created Child Process with pid [%d]\n",__FUNCTION__, pid);
    gChildTable[index].pid = pid;
    gChildTable[index].state = ON;
	}
	return 0;
}

void childProcess()
{
	int ret;
	struct sigaction sigact;
  /*register child signal handler*/
  sigact.sa_sigaction = childSignalHandler;
  sigact.sa_flags = SA_SIGINFO;
  ret = sigaction(SIGINT, &sigact, NULL);
	if(ret < 0)
	{
		fprintf(stderr,"%s : Error in registering child signal handler !!!\n", __FUNCTION__);
		exit(1);
	}

	while(1)
	{
		printf("Hi, Welcome aboard on Flight Number [%d]\n", getpid());
		sleep(5);
	}
}

void parentSignalHandler(int signum, siginfo_t *si, void *uContext)
{
	int i,ret;
	printf("%s : Received signal [%d] [%s] in parent [%d]!!!\n", __FUNCTION__, signum ,strsignal(signum), getpid());
	gExitFlag = 1;
	if(signum == SIGCHLD)
	{
		printf("%s : SIGCHLD received si_code [%d] si_pid [%d] !!!\n", __FUNCTION__, si->si_code, si->si_pid);
		return;
	}
	for(i=0; i<NUM_CHILD; i++)
	{
		printf("%s : Sending signal [%d] [%s] to child [%d]\n", __FUNCTION__, signum , strsignal(signum), gChildTable[i].pid);
		if(gChildTable[i].state == ON)
		{
			ret = kill(gChildTable[i].pid, signum);
			if(ret < 0)
			{
				fprintf(stderr, "%s : Error [%d] [%s] in kill !!!\n", __FUNCTION__, errno, strerror(errno));
			}
		}
	}
	signal(signum,SIG_DFL);
	raise(signum);
}

void childSignalHandler(int signum, siginfo_t *si, void *uContext)
{
	printf("%s : Received signal [%d] [%s] in child [%d]!!!\n", __FUNCTION__, signum ,strsignal(signum), getpid());
	gExitFlag = 1;
	signal(signum, SIG_DFL);
	raise(signum);
}
