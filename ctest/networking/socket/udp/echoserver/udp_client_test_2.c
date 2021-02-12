#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <pthread.h>

#ifdef DEBUG
	#define DEBUG_PRINTF(format, ...) \
		printf("dbg: %s():%s:%u "format"\n", \
				__func__, __FILE__, __LINE__, ##__VA_ARGS__)
#else
	#define DEBUG_PRINTF(format, ...)
#endif 


struct udp_msg_multiplexer {
	int udp_mmp_sock;
	pthread_t udp_mmp_rx_thread;
};

void *udp_mmp_msg_rx_thread(void *arg)
{
#define MSG_SZ 10480
	struct udp_msg_multiplexer *udp_mmp = arg;
	struct sockaddr_in remote_addr;
	socklen_t remote_addr_len = sizeof(remote_addr);
	char msg[MSG_SZ];
	int bytes;

	while (1) {
		bytes = recvfrom(udp_mmp->udp_mmp_sock, msg, MSG_SZ, 0,
				(struct sockaddr *)&remote_addr, &remote_addr_len);
		if (bytes < 0) {
			printf("error in recvfrom() %d %s", errno, strerror(errno));
			continue;
		}
		msg[bytes] = '\0';
		printf("rcvd msg of len %d from %s:%d\n%s\n", bytes,
			inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port), msg);
	}
#undef MSG_SZ
}

int udp_mmp_init(struct udp_msg_multiplexer *udp_mmp)
{
	int sock;

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		printf("error in socket() %d %s\n", errno, strerror(errno));
		return 1;
	}
	udp_mmp->udp_mmp_sock = sock;

	if (pthread_create(&(udp_mmp->udp_mmp_rx_thread), NULL,
							udp_mmp_msg_rx_thread, udp_mmp)) {
		printf("error in pthread_create() %d %s\n", errno, strerror(errno));
		close(sock);
		return 2;
	}
	return 0;
}

int udp_mmp_msg_tx(struct udp_msg_multiplexer *udp_mmp,
					char *remote_ip, uint16_t remote_port,
					char *msg, uint32_t msg_len)
{
	struct sockaddr_in remote_addr;
	socklen_t remote_addr_len;
	int bytes;

	remote_addr.sin_family = AF_INET;
	remote_addr.sin_port = htons(remote_port);
	if (!inet_aton(remote_ip, &(remote_addr.sin_addr))) {
		printf("Error in inet_aton()\n");
		return 1;
	}

	remote_addr_len = sizeof(remote_addr);
	bytes = sendto(udp_mmp->udp_mmp_sock, msg, msg_len, 0,
			(struct sockaddr *)&remote_addr, remote_addr_len);

	if (bytes < 0) {
		DEBUG_PRINTF("error in sendto() %d %s", errno, strerror(errno));
		return 2;
	}
	printf("sent msg of len %d to %s:%d\n", bytes,
			inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port));

	return 0;
}

void *udp_msp_sender_thread_1(void *arg)
{
	struct udp_msg_multiplexer *udp_mmp = arg;
	char *msg = "mohit singh 9955";

	while (1) {
		if (udp_mmp_msg_tx(udp_mmp, "127.0.0.1", 9955, msg, strlen(msg))) {
			printf("Error in udp_mmp_msg_tx()\n");
		}
		usleep(1000);
	}
}

void *udp_msp_sender_thread_2(void *arg)
{
	struct udp_msg_multiplexer *udp_mmp = arg;
	char *msg = "mohit singh 9966";
	while (1) {
		if (udp_mmp_msg_tx(udp_mmp, "127.0.0.1", 9966, msg, strlen(msg))) {
			printf("Error in udp_mmp_msg_tx()\n");
		}
		usleep(1000);
	}
}


int main(int argc, char *argv[])
{
	struct udp_msg_multiplexer udp_mmp;
	pthread_t sender_thread1, sender_thread2;

	if (udp_mmp_init(&udp_mmp)) {
		printf("Error in udp_mmp_init()\n");
		exit(1);
	}

	if (pthread_create(&sender_thread1, NULL, 
				udp_msp_sender_thread_1, &udp_mmp)) {
		printf("error in pthread_create() %d %s\n", errno, strerror(errno));
		exit(2);
	}

	if (pthread_create(&sender_thread2, NULL, 
				udp_msp_sender_thread_2, &udp_mmp)) {
		printf("error in pthread_create() %d %s\n", errno, strerror(errno));
		exit(3);
	}

	pthread_join(udp_mmp.udp_mmp_rx_thread, NULL);
	
	return 0;
}
