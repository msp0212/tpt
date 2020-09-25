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

int create_udp_server(char *srvr_ip, uint16_t srvr_port)
{
	struct sockaddr_in srvr_addr;
	int srvr_sock;

	srvr_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (srvr_sock < 0) {
		printf("error in socket() %d %s\n", errno, strerror(errno));
		return -1;
	}

	int val = 1;
	setsockopt(srvr_sock, SOL_SOCKET, SO_REUSEADDR, (void *)&val, sizeof(val));

	srvr_addr.sin_family = AF_INET;
	srvr_addr.sin_port = htons(srvr_port);
	if (!inet_aton(srvr_ip, &(srvr_addr.sin_addr)))	{
		printf("error in inet_aton()\n");
		return -2;
	}
	if (bind(srvr_sock, (struct sockaddr *)&srvr_addr, sizeof(srvr_addr)) < 0) {
		printf("error in bind() %d %s\n", errno, strerror(errno));
		return -3;
	}
	return srvr_sock;
}

#define BUF_SZ 2048
void data_rx_tx_loop(int srvr_sock)
{
	struct sockaddr_in clt_addr;
	socklen_t clt_addr_len;
	int bytes;
	uint8_t buf[BUF_SZ];

	clt_addr_len = sizeof(clt_addr);
	while (1) { 
		bytes = recvfrom(srvr_sock, buf, BUF_SZ, 0,
							(struct sockaddr *)&clt_addr, &clt_addr_len);
		if (bytes < 0) {
			DEBUG_PRINTF("error in recvfrom() %d %s", errno, strerror(errno));
			continue;
		}
		buf[bytes] = '\0';
		printf("rcvd msg of len %d from %s:%d\n%s\n", bytes,
				inet_ntoa(clt_addr.sin_addr), ntohs(clt_addr.sin_port), buf);

		bytes = sendto(srvr_sock, buf, bytes, 0,
							(struct sockaddr *)&clt_addr, clt_addr_len);
		if (bytes < 0) {
			DEBUG_PRINTF("error in sendto() %d %s", errno, strerror(errno));
		}
		printf("sent back msg of len  %d\n to %s:%d\n", bytes,
				inet_ntoa(clt_addr.sin_addr), ntohs(clt_addr.sin_port));
	}

}

int main(int argc, char *argv[])
{
	char srvr_ip[16];
	uint16_t srvr_port;
	int srvr_sock;

	if (process_command_line_args(argc, argv, srvr_ip, &srvr_port)) {
		printf("Error in process_command_line_args()\n");
		exit(1);
	}

	srvr_sock = create_udp_server(srvr_ip, srvr_port);
	if (srvr_sock < 0) {
		printf("Error in create_udp_server()\n");
	}

	data_rx_tx_loop(srvr_sock);

	return 0;
}
