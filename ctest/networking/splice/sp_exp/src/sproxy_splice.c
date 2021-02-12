/*system headers*/
#define _GNU_SOURCE
#include <fcntl.h>
#include<string.h>
#include<errno.h>
#include <poll.h>
/*Local headers*/
#include<sproxy_splice.h>
#include<sproxy_log.h>
#include<sproxy_globals.h>

static int splice_with_poll(int pipe_fd, int sock_fd, size_t len);

int make_splice(int fd_in, int fd_out, int *pipefd_arr, size_t len)
{
	int ret = 0;
	if (pipefd_arr == NULL) {
		log_msg(LOG_CRIT,"NULL params received !!!");
		return -1;
	}
	while (1) {	
		ret = splice(fd_in, NULL, pipefd_arr[1], NULL, len,  SPLICE_F_MOVE | SPLICE_F_NONBLOCK);
		if (ret < 0) {
			if (errno == EAGAIN) {
				ret = 1;
				log_msg(LOG_NOTICE, "splicing S->P will block now...exiting from while loop and polling again !!!");
			}
			else {
				ret = -1;
				log_msg(LOG_ERR, "splice Error [%d] [%s]", errno, strerror(errno));
			}
			goto label;
		}
		else if(ret == 0) {
			log_msg(LOG_INFO, "NOTHING to splice !!!");
			goto label;
		}
		ret = splice_with_poll(pipefd_arr[0], fd_out, len);
		if (ret < 0) {
			ret = -1;
			log_msg(LOG_ERR, "Error in splice_with_poll P->S!!!", errno, strerror(errno));
			goto label;
		}
		else if(ret == 0) {
			goto label;
		}
		log_msg(LOG_DEBUG, "spliced [%d] bytes from OS to client", ret);
	}
label :
	return ret;
}

static int splice_with_poll(int fd_in, int fd_out, size_t len)
{
	int ret = 0;
	struct pollfd fd_array[1];

	fd_array[0].fd = fd_out;
	fd_array[0].events = POLLOUT;
	while (1) {
		ret = poll(fd_array, 1, SPROXY_POLL_TIMEOUT);
		if (ret < 0) {
			if (errno == EINTR){
				continue;
			}
			else {
				log_msg(LOG_ERR, "Error [%d] [%s] in poll !!!", errno, strerror(errno));
				ret = -1;
				goto label;
			}
		}
		else if( ret == 0 )/*timed out and no descriptors were ready*/ {
			log_msg(LOG_ERR, "Poll timed out !!!")
			ret = -1;
			goto label;
		}
		else if (ret > 0 && (fd_array[0].revents & POLLOUT)) /*write will not block now*/ {
			ret = splice(fd_in, NULL, fd_out, NULL, len,  SPLICE_F_MOVE | SPLICE_F_NONBLOCK);
			if (ret < 0) {
				log_msg(LOG_ERR, "splice Error [%d] [%s]", errno, strerror(errno));
				ret = -1;
			}
			else if (ret == 0) {
				log_msg(LOG_INFO, "NOTHING to splice !!!");
			}
			goto label;
		}
	}
label:
	return ret;
}
