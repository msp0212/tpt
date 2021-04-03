#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define ALLOC_SIZE 4096
#define REALLOC_THRESHOLD 128
#define READ_FILE_NAME "./logs"
#define WRITE_FILE_NAME "./copy"

int main()
{
	char *buf = NULL;
	long int  buf_size = 0;
	long int buf_len = 0;
	int bytes_read = 0;
	int bytes_written = 0;
	int read_fd;
	int write_fd;

	buf_size = ALLOC_SIZE;
	buf = malloc(sizeof(*buf) * buf_size);

	read_fd = open(READ_FILE_NAME, O_RDONLY);
	if (read_fd < 0) {
		fprintf(stderr,"Error [%d] [%s] in open\n",
					errno, strerror(errno));
		exit(1);
	}
	while(1) {
		bytes_read = read(read_fd, buf + buf_len, buf_size - buf_len);
		if (bytes_read < 0) {
			fprintf(stderr,"Error [%d] [%s] in read\n",
					errno, strerror(errno));
			break;
		} else if (bytes_read == 0) {
			printf("EOF is reached\n");
			break;
		} else {
			buf_len += bytes_read;
			printf("bytes_read %d, buf_len %ld\n",
						bytes_read, buf_len);
			if (buf_size - buf_len <= REALLOC_THRESHOLD) {
				buf_size *= 2;
				buf = realloc(buf, buf_size);
				printf("realloced the buffer, buf_size %ld\n", 
								buf_size);
			}
		}
	}
	write_fd = open(WRITE_FILE_NAME, O_WRONLY|O_CREAT, S_IWUSR);
	if (write_fd < 0) {
		fprintf(stderr,"Error [%d] [%s] in open\n",
					errno, strerror(errno));
		exit(1);
	}
	bytes_written = write(write_fd, buf, buf_len);
	if (bytes_written < 0) {
		fprintf(stderr,"Error [%d] [%s] in write\n",
                                        errno, strerror(errno));
	}
	printf("bytes_written %d\n", bytes_written);
	return 0;
}
