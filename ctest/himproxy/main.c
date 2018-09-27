/*System Headers*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
/*Local Headers*/
#include <log.h>
#include <globals.h>
#include <network.h>
#include <core.h>
#include <signal_handler.h>


static char listen_ip[16] = "0.0.0.0";
static int listen_port = 5401;

static int parse_command_line(int argc, char *argv[]);
static void print_help(char *str);
static int wait_for_child(int (*start_child)(int));
static int handle_child_exit(pid_t pid);

int main(int argc, char *argv[]) 
{
	int ret = 0;

	if ((ret = parse_command_line(argc, argv)) < 0) {
		exit(EXIT_FAILURE);
	}

	open_system_logger(argv[0], g_log_level);
	
	register_all_signal_handler(parent_signal_handler);

	if ((ret = tcp_bind_and_listen(listen_ip, listen_port, 
						&g_server_sock)) < 0) {
		log_msg(LOG_EMERG, "Error in tcp_bind_and_listen !!!");
		exit(EXIT_FAILURE);
	}
	if (set_socket_non_blocking(&g_server_sock) < 0) {
		log_msg(LOG_EMERG, "Error in set_socket_non_blocking !!!");
		exit(EXIT_FAILURE);
	}
	if (set_socket_ip_transparent(&g_server_sock) < 0) {
		log_msg(LOG_EMERG, "Error in set_socket_ip_transparent !!!");
		exit(EXIT_FAILURE);
	}
	log_msg(LOG_INFO, "HimProxy listening on [%s:%d]", listen_ip, listen_port);

	create_multiple_child(g_num_child);

	wait_for_child(start_child);

	return 0;
}

static int wait_for_child(int (*start_child)(int))
{
	pid_t pid;
	int status;

	while (g_active_child_count > 0) {
		pid = waitpid(-1, &status, WUNTRACED);
		if(pid < 0) {
			if (errno == EINTR) {
				continue;
			} else {
				log_msg(LOG_ALERT, "Error [%d] [%s] in waitpid"
						, errno, strerror(errno));
				break;
			}
		} else {
			log_msg(LOG_CRIT, "child [%d] exit status [%d]",
					pid, status);
			if (status > 0) {
				handle_child_exit(pid);
			} else {
				g_active_child_count--;
			}
			log_msg(LOG_DEBUG, "active count [%d]", g_active_child_count);
		}
	}
	return 0;
}


static int handle_child_exit(pid_t pid)
{
	int i = 0;

	for (i = 0; i < g_num_child; i++) {
		if (pid == g_child_table[i].pid) {
			log_msg(LOG_ALERT, "Restarting process"
					"on index [%d]", i);
			(*start_child)(i);
		}
	}
	return 0;
}


static int parse_command_line(int argc, char *argv[])
{
	int c = 0;
	int ret = 0;
	while((c = getopt(argc, argv, ":hi:l:p:v")) > 0) {
		switch(c) {
		case 'h' :
			print_help(argv[0]);
			break;
		case 'i' :
			strncpy(listen_ip, optarg, sizeof(listen_ip));
			break;
		case 'l' :
			g_log_level = atoi(optarg);
			break;
		case 'n' :
			g_num_child = atoi(optarg);
			break;
		case 'p' :
			listen_port = atoi(optarg);
			break;
		case 'v' :
			printf("%s\n", HIMPROXY_VERSION_DETAILS);
			break;
		case '?' :
			fprintf(stderr, "%s: Invalid option argument -- '%c'\n",
						argv[0], optopt);
			print_help(argv[0]);
			ret = -1;
			break;
		case ':' :
			fprintf(stderr, "%s: Option requires an argument -- '%c'\n",
								argv[0], optopt);
			print_help(argv[0]);
			ret = -1;
			break;
		default:
			print_help(argv[0]);
			break;
		}
	}
	return ret;
}

static void print_help(char *str)
{
	printf("Usage: \n"
		"%s OPTIONS\n"
		"Available Options:\n"
		"-h this text\n"
		"-i listen_ip\n"
		"-l log_level\n"
		"-n num_children\n"
		"-p listen_port\n"
		"-v version info\n", str);
}
