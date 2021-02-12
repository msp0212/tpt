#ifndef PROXY_CONFIG_H
#define PROXY_CONFIG_H

#define PROXY_SECTION_NAME "PROXY"

struct proxy_config {
	char listen_ip[16];
	int listen_port;
	int log_level;
	int child_count;
	int workq_parallelism;
	int workq_thread_timeout;
	int workq_length;
	int ip_transparency;
};

int get_config();
int free_config();
#endif
