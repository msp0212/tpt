/*System Headers*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include <signal.h>
/*Local Headers*/
#include<sproxy_globals.h>
#include<workq.h>
#include<sproxy_log.h>
#include<sproxy_net.h>
#include<sproxy_core.h>

static void handle_signal(int signo);

static int init_workq();

int main(int argc, char **argv)
{
	signal(SIGINT,handle_signal);

	int ret = 0, len = 0;
	char bind_ip[16] = {0,};
	int client_socket , bind_port = 0;
	struct sockaddr_in client_address;
	struct Proxy_Info *proxy_info = NULL;
	struct linger linger;
	int log_level;

	if(argc < 7)
	{
		fprintf(stderr, "%s : Insufficient arguments to program !!!\n", __FUNCTION__);
		fprintf(stderr, "Usage : %s <bind_ip> <bind_port> <dest_ip> <dest_port> <splice_size> <log_level>\n", argv[0]);
		exit(1);
	}

	strncpy(bind_ip, argv[1], sizeof(bind_ip));
	bind_port = atoi(argv[2]);
	strncpy(g_dest_ip, argv[3], sizeof(g_dest_ip));
	g_dest_port = atoi(argv[4]);
	g_splice_size = atoi(argv[5]);
	log_level = atoi(argv[6]);

	open_system_logger(argv[0], log_level);

	ret = init_workq();
	if (ret != 0) {
		log_msg(LOG_EMERG, "Error in init_workq !!!");
		exit(1);
	}

	ret = tcp_bind_and_listen(bind_ip, bind_port, &g_server_socket);
	if (ret != 0) {
		log_msg(LOG_EMERG, "Error in server_bind_and_listen!!!");
		exit(1);
	}

	linger.l_onoff = 0;
	linger.l_linger = 0;
	log_msg(LOG_INFO, "Waiting for client connections...");
	while (1) {
		len = sizeof(client_address); 
		client_socket = accept(g_server_socket, (struct sockaddr *)&client_address, (socklen_t *)&len);
		if (client_socket < 0) {
			log_msg(LOG_ALERT, "Accept Error [%d] [%s]", errno, strerror(errno));
			continue;
		}
		
		ret = set_socket_non_blocking(&client_socket);
		if (ret < 0) {
			log_msg(LOG_ALERT, "Error in set_socket_non_blocking !!!", errno, strerror(errno));
			close(client_socket);
			continue;
		}
		
		setsockopt(client_socket, SOL_SOCKET, SO_LINGER , (char *)&linger, sizeof(linger));

		proxy_info = (struct Proxy_Info*)calloc(1, sizeof(struct Proxy_Info));
		proxy_info->client_socket = client_socket;
		memcpy(&(proxy_info->client_address), &client_address, sizeof(proxy_info->client_address));
	
		log_msg(LOG_INFO, "Adding to tunnel workq...");
		if (WorkQAdd(&tunnel_workq, (void*)proxy_info) != 0 ) {
			log_msg(LOG_ALERT, "Error in WorkQAdd for client_handler!!!");
		}
	}
	return 0;	
}

static int init_workq()
{
	if (WorkQInit(&tunnel_workq, 200, 5, tunnel_the_request) != 0) {
		log_msg(LOG_EMERG, "WorkQInit failed for tunnel workq");
		return -1;
	}
	return 0;
}


static void handle_signal(int signo)
{
	if (signo == SIGINT) {
		log_msg(LOG_EMERG, "Recieved SIGINT...Exiting now")
	}
	/*close server socket*/
	close(g_server_socket);
	/*close the log*/
	close_system_logger();
	exit(0);
}
