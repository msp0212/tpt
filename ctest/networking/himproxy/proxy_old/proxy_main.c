/*System Headers*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
/*Common Headers*/
#include<sample.h>
/*Local Headers*/
#include<proxy_log.h>
#include<proxy_core.h>
#include<proxy_globals.h>
#include<proxy_signal_handler.h>
#include<proxy_config.h>
#include<proxy_net.h>

char conf_file[32] = {0,};

static void print_help(char *prog_name);
static int parse_command_line_arguments(int argc, char **argv);
static int wait_for_child(int (*start_child)(int));
static int handle_child_exit(pid_t pid);

int main(int argc, char **argv)
{
	int ret = 0;

	parse_command_line_arguments(argc, argv);
	
	if (get_config(conf_file) < 0) {
		fprintf(stderr, "%s : Error in get_config !!!\n", __FUNCTION__);
		exit(1);
	}
	
	open_system_logger(argv[0], g_proxy_config.log_level);
	
	register_all_signal_handler(parent_signal_handler);
	
	ret = tcp_bind_and_listen(g_proxy_config.listen_ip, 
				g_proxy_config.listen_port, &g_server_sock);
	log_msg(LOG_INFO, "Proxy listening on [%s : %d]",
					g_proxy_config.listen_ip,
					g_proxy_config.listen_port);
	if (ret < 0) {
		log_msg(LOG_EMERG, "Error in tcp_bind_and_listen !!!");
		exit(1);
	}

	create_multiple_child(g_proxy_config.child_count);
        
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

	for (i = 0; i < g_proxy_config.child_count; i++) {
		if (pid == g_child_table[i].pid) {
			log_msg(LOG_ALERT, "Restarting process"
					"on index [%d]", i);
				(*start_child)(i);
			}
	}
	return 0;
}

static int parse_command_line_arguments(int argc, char **argv) 
{
    int c = 0; 
    opterr = 0; /*don't allow getopt() to throw error message*/
    while ((c = getopt(argc, argv, ":c:vh")) > 0) {
        switch (c) {
            case 'c' :
			strncpy(conf_file, optarg, sizeof(conf_file));
                break;
            case 'v' :
                printf("%s\n", PROXY_VERSION_DETAILS);
                break;
            case 'h' :
                print_help(argv[0]);
                break;
            case '?' :
			printf("Invalid Option %c !!!\n", optopt);
                print_help(argv[0]);
                break;    
            case ':' :
			printf("Missing argument for option %c\n", optopt);
                print_help(argv[0]);
                break;
            default :
                print_help(argv[0]);
                break;
        }
    }
    return 0;  
}

static void print_help(char *prog_name)
{
    printf("USAGE :\n"
            "./%s OPTION\n"
            "Available Options : \n"
            "-c <pathname of the conf file>  proxy configuration file\n"
            "-v print the version details\n"
            "-h Help text\n", prog_name);
}

