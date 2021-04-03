#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "driver_ioctl.h"

int main(int argc, char *argv[])
{
	char *filename = "/dev/driver";
	test_ioctl_t io;
	int fd;
	int op;


	if (argc != 2) {
		fprintf(stderr, "Wrong no of arguments!\n");
		printf("Usage: %s <1-get, 2-set>\n", argv[0]);
		exit(1);
	}
	op = atoi(argv[1]);
	
	fd = open(filename, O_RDWR);
	if (fd == -1) {
		fprintf(stderr, "Error in fopen\n");
		exit(2);
	}
	
	switch(op) {
	case 1:
		if (ioctl(fd, TEST_GET, &io) != 0) {
			fprintf(stderr, "Error in ioctl get\n");
		}
		printf("ioctl get result : %d %d\n", io.code, io.val);
		break;
	case 2:
		printf("enter code:");
		scanf("%d", &io.code);
		printf("enter val:");
		scanf("%d", &io.val);
		if (ioctl(fd, TEST_SET, &io) != 0) {
			fprintf(stderr, "Error in ioctl set\n");
		}
		printf("ioctl set done!\n");
		break;
	default:
		printf("unknown operation\n");
		break;
	}
	return 0;
}
