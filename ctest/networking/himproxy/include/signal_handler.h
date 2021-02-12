#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H
#include<signal.h>
int register_signal_handler(int signum, void  (*signal_handler)(int, 
				siginfo_t *, void *));
int register_all_signal_handler(void (*signal_handler)(int, siginfo_t *, 
					void *));
void parent_signal_handler(int signum, siginfo_t *si, void *ucontext);
void child_signal_handler(int signum, siginfo_t *si, void *ucontext);
#endif
