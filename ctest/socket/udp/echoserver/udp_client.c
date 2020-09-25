#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <netinet/in.h>

#ifdef DEBUG
	#define DEBUG_PRINTF(format, ...) \
		printf("dbg: %s():%s:%u "format"\n", \
				__func__, __FILE__, __LINE__, ##__VA_ARGS__)
#else
	#define DEBUG_PRINTF(format, ...)
#endif 



int process_command_line_args(int argc, char *argv[],
								char *srvr_ip, uint16_t *srvr_port)
{
	if (argc != 3) {
		printf("Wrong number of args passed!\n"
				"Usage: %s <srvr_ip> <srvr_port>\n", argv[0]);
		return 1;
	}
	strcpy(srvr_ip, argv[1]);
	*srvr_port = atoi(argv[2]);
	return 0;
}

#define BUF_SZ 2048
int data_tx_rx(int sock, char *remote_ip, uint16_t remote_port)
{
	struct sockaddr_in remote_addr;
	socklen_t remote_addr_len;
	int bytes;
	char buf[BUF_SZ];
	
	strcpy(buf, "mohit singh test");

	remote_addr.sin_family = AF_INET;
	remote_addr.sin_port = htons(remote_port);
	if (!inet_aton(remote_ip, &(remote_addr.sin_addr))) {
		printf("Error in inet_aton()\n");
		return 1;
	}

	remote_addr_len = sizeof(remote_addr);
	bytes = sendto(sock, buf, strlen(buf), 0,
			(struct sockaddr *)&remote_addr, remote_addr_len);
	if (bytes < 0) {
		DEBUG_PRINTF("error in sendto() %d %s", errno, strerror(errno));
		return 2;
	}
	printf("sent msg of len  %d\n to %s:%d\n", bytes,
			inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port));

	bytes = recvfrom(sock, buf, BUF_SZ, 0,
			(struct sockaddr *)&remote_addr, &remote_addr_len);
	if (bytes < 0) {
		DEBUG_PRINTF("error in recvfrom() %d %s", errno, strerror(errno));
		return 3;
	}
	buf[bytes] = '\0';
	printf("rcvd msg of len %d from %s:%d\n%s\n", bytes,
			inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port), buf);
	return 0;
}

int main(int argc, char *argv[])
{
	char srvr_ip[16];
	uint16_t srvr_port;
	int sock;

	if (process_command_line_args(argc, argv, srvr_ip, &srvr_port)) {
		printf("Error in process_command_line_args()\n");
		exit(1);
	}

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		printf("error in socket() %d %s\n", errno, strerror(errno));
		exit(2);
	}

	if (data_tx_rx(sock, srvr_ip, srvr_port)) {
		printf("error in data_tx_rx()\n");
		exit(3);
	}
	return 0;
}
