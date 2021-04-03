
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	int sock, length;
	struct sockaddr_un name;
	char buf[1024];
	int len;
	/* Create socket from which to read. */
	sock = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sock < 0) {
		perror("opening datagram socket");
		exit(1);
	}

	/* Create name. */
	name.sun_family = AF_UNIX;
	strcpy(name.sun_path, "myUnixSocket2");
	len = strlen(name.sun_path) + sizeof(name.sun_family);

	if (bind(sock, (struct sockaddr *)&name, SUN_LEN(&name))) {
		perror("binding name to datagram socket");
		exit(1);
	}

	/* Read from the socket. */
	if (read(sock, buf, 1024) < 0)
		perror("receiving datagram packet");
	printf("-->%s\n", buf);
	sleep(3600);
	close(sock);
}
