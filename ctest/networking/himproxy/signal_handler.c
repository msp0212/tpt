/*
 * This file contains signal handling part
 * */
/*Sytem Headers*/
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<errno.h>
/*Local Header*/
#include<signal_handler.h>
#include<log.h>
#include<globals.h>
#include<core.h>

/*
 *TODO check whether all signal handlers are reentrant or not ?
 * */

static int kill_all_children(int signum);

int register_signal_handler(int signum, void (*signal_handler)(int, siginfo_t *
				,void *))
{
	int ret = 0;
	struct sigaction sigact;
	sigact.sa_sigaction = signal_handler;
	sigact.sa_flags = SA_SIGINFO;
	sigemptyset(&sigact.sa_mask);
	ret = sigaction(signum, &sigact, NULL);
	if(ret < 0) {
		log_msg(LOG_CRIT, "Error [%d] [%s] in registering "
				"signal handler for signal [%d] !!!", 
				errno, strerror(errno), signum);
		ret = -1;
	}
	return ret;
}

int register_all_signal_handler(void (*signal_handler)(int, siginfo_t *,
					void *)) 
{	
	int i;
	/*only the signals defined in POSIX.1-1990 standard*/
	int signums[] = {SIGHUP, SIGINT, SIGQUIT, SIGILL, SIGTRAP, SIGABRT, 
			 SIGFPE, SIGUSR1, SIGSEGV, SIGUSR2, SIGPIPE, SIGALRM,
			 SIGCHLD, SIGCONT, /*SIGSTOP,*/ SIGTSTP, SIGTTIN,
			 SIGTTOU };
	int size = sizeof(signums)/sizeof(signums[0]);
	for (i = 0; i < size; i++) {
		register_signal_handler(signums[i], signal_handler);
	}
	return 0;
}

/*call only signal safe functions inside signal handler*/
void parent_signal_handler(int signum, siginfo_t *si, void *ucontext)
{
	int ret = 0;

	/*FIXME Why SA_NODEFER does not work ?*/
      	
      	/*
	 *Code for catchng signal within its own signal handler
	 */

	/*
	sigset_t mask, old;
	sigprocmask(1,NULL, &old);
	mask = old;
	sigaddset(&mask, 2);
	sigprocmask(SIG_UNBLOCK, &mask, &old);
	if (sigismember(&mask, 2)) {
		log_msg(LOG_EMERG, "SIGKILL is blocked !!!");
	} else {
		log_msg(LOG_EMERG, "SIGKILL is not blocked !!!");
	}*/

	log_msg(LOG_EMERG, "Received signal [%d] [%s] in parent [%d]", signum,
				strsignal(signum), getpid());
	if (signum == SIGCHLD) {
		log_msg(LOG_EMERG, "SIGCHLD received.si_signo [%d] si_code "
				"[%d] si_pid [%d] !!!", si->si_signo, 
				si->si_code, si->si_pid);
		
	} else {
		ret = kill_all_children(signum);
		if (ret < 0) {
			log_msg(LOG_CRIT,"Error in kill_all_children !!!\n");
		}
		proxy_parent_cleanup();
	}
	signal(signum, SIG_DFL);
	raise(signum);
	return;
}

static int kill_all_children(int signum)
{
	int ret = 0;
	int i = 0;
	for (i = 0; i < g_num_child; i++) {
		log_msg(LOG_EMERG, "Sending signal [%d] [%s] to"
				" child [%d]", signum, strsignal(signum),
				g_child_table[i].pid);
		ret = kill(g_child_table[i].pid, signum);
		if(ret < 0){
			log_msg(LOG_CRIT, "Error [%d] [%s] in kill",
					errno, strerror(errno));
			ret = -1;
		}
	}
	return ret;
}

void child_signal_handler(int signum, siginfo_t *si, void *ucontext) 
{
	log_msg(LOG_EMERG, "Received signal [%d] [%s] in child [%d] !!!", 
				signum ,strsignal(signum), getpid());
	proxy_child_cleanup();
	signal(signum, SIG_DFL);
	raise(signum);
	return;
}
