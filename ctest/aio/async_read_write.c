#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <aio.h>
//#include <aiocb.h>

#define ALLOC_SIZE 4096
#define REALLOC_THRESHOLD 128
#define READ_FILE_NAME "./logs"
#define WRITE_FILE_NAME "./copy"

struct read_info {
	int fd;
	char *buf;
	long int buf_size;
	long int buf_len;
	struct aiocb cb;
	struct sigevent sigev;
	pthread_attr_t attr;
};

void read_callback(union sigval);

int main()
{
	struct read_info *ri = NULL;
	int ret = 0;

	ri = calloc(1, sizeof(*ri));
	ri->buf_size = ALLOC_SIZE;
	ri->buf = malloc(sizeof(*ri->buf) * ri->buf_size);

	ri->fd = open(READ_FILE_NAME, O_RDONLY);
	if (ri->fd < 0) {
		fprintf(stderr,"Error [%d] [%s] in open\n",
					errno, strerror(errno));
		free(ri->buf);
		free(ri);
		exit(1);
	}

	pthread_attr_init(&ri->attr);
	pthread_attr_setdetachstate(&ri->attr, PTHREAD_CREATE_DETACHED);

	ri->sigev.sigev_notify = SIGEV_THREAD;
	ri->sigev.sigev_value.sival_ptr = ri;
	ri->sigev.sigev_notify_function = read_callback;
	ri->sigev.sigev_notify_attributes = (void*)&ri->attr;

	memset(&ri->cb, 0, sizeof(ri->cb));
	ri->cb.aio_fildes = ri->fd;
	ri->cb.aio_buf = ri->buf;
	ri->cb.aio_nbytes = ri->buf_size;
	ri->cb.aio_sigevent = ri->sigev; 
	
	ret = aio_read(&ri->cb);
	if (ret < 0) {
		fprintf(stderr, "Error %d %s in aio_read\n",
					errno, strerror(errno));
		exit(1);
	}
	ret = aio_error(&ri->cb);
	if (ret == 0) {
		printf("aio_read Success\n");
	} else if (ret == ECANCELED) {
		printf("aio_read canceled");
	} else if (ret == EINPROGRESS){
		printf("read operation in progress\n");
	}else {
		fprintf(stderr, "Error %d %s in aio_read\n", errno, strerror(errno));
		exit(1);
	}
	pthread_exit(NULL);
	return 0;
}


void read_callback(union sigval sigev_value) 
{
	struct read_info *ri = NULL;
	int ret = 0;
	int write_fd;
	int bytes_written;
	
	ri = sigev_value.sival_ptr;
	printf("%s invoked\n", __FUNCTION__);

	ret = aio_return(&ri->cb);
	if (ret < 0) {
		printf("Error %d %s in async read\n", errno, strerror(errno));
	} else if (ret == 0) {
		printf("EOF is reached\n");
		write_fd = open(WRITE_FILE_NAME, O_WRONLY|O_CREAT, S_IWUSR);
		if (write_fd < 0) {
			fprintf(stderr,"Error [%d] [%s] in open\n",
					errno, strerror(errno));
			exit(1);
		}
		bytes_written = write(write_fd, ri->buf, ri->buf_len);
		if (bytes_written < 0) {
			fprintf(stderr,"Error [%d] [%s] in write\n",
					errno, strerror(errno));
		}
		printf("bytes_written %d\n", bytes_written);

	} else {
		ri->buf_len += ret;
		printf("bytes_read %d, buf_len %ld\n",
				ret, ri->buf_len);
		if (ri->buf_size - ri->buf_len <= REALLOC_THRESHOLD) {
			ri->buf_size *= 2;
			ri->buf = realloc(ri->buf, ri->buf_size);
			printf("realloced the buffer, buf_size %ld\n",
					ri->buf_size);
		}
		ri->cb.aio_offset = ri->buf_len;
		ri->cb.aio_nbytes = ri->buf_size - ri->buf_len;
		ri->cb.aio_buf = ri->buf + ri->buf_len;
		ret = aio_read(&ri->cb);
		if (ret < 0) {
			fprintf(stderr, "Error %d %s in aio_read\n",
					errno, strerror(errno));
			exit(1);
		}

		ret = aio_error(&ri->cb);
		if (ret == 0) {
			printf("aio_read Success\n");
		} else if (ret == ECANCELED) {
			printf("aio_read canceled");
		} else if (ret == EINPROGRESS){
			printf("read operation in progress\n");
		}else {
			fprintf(stderr, "Error %d %s in aio_read\n", errno, strerror(errno));
			exit(1);
		}
	}
}
