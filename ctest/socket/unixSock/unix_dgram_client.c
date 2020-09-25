#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>

#define DATA "This is a test msg !\n"

int main()
{
	int sock;
	struct sockaddr_un name;
	/* Create socket on which to send. */
	sock = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sock < 0) {
		perror("opening datagram socket");
		exit(1);
	}
	/* Construct name of socket to send to. */
	name.sun_family = AF_UNIX;
	strcpy(name.sun_path, "myUnixSocket2");

	/* Send message. */
	while (1) {
		printf("sending msg...\n");
		if (sendto(sock, DATA, sizeof(DATA), 0, (struct sockaddr *)&name,
					sizeof(struct sockaddr_un)) < 0) {
			perror("sending datagram message");
		}
		printf("msg sent.\n");
	}
	close(sock); 
}
