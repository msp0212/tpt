/*System Headers*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <poll.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
/*Local Headers*/
#include <conn_mgr_conn.h>
#include <conn_mgr_errors.h>

/**@struct conn_mgr_conn_t
 * @brief Structure describing a connection
 * */
struct conn_mgr_conn_t {
	int fd; /**<fd associated with the connection*/
};

static int set_socket_non_blocking(int fd);
static int set_socket_blocking(int fd);
static int tcp_connect_with_timeout(int fd, struct sockaddr_in addr, 
								int timeout);
static int poll_for_connect(int fd, int timeout);

/**@brief Get a new connection object 
 * @param[in] ip IP address to connect
 * @param[in] port Port to connect
 * @param[in] timeout connect timeout in ms
 * @return On success, pointer to a new allocated conn object
 *	   On failure, NULL and conn_mgr_conn_errno is set
 * */
conn_mgr_conn_t *conn_mgr_conn_new(char *ip, int port, int timeout)
{
	conn_mgr_conn_t *conn  = NULL;
	struct sockaddr_in addr;
	int fd;
	int ret = 0;

	if (ip == NULL) {
		goto exit;
	}
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		conn_mgr_conn_errno = CONN_MGR_CONN_ERR_SOCKET;
		goto exit;
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	inet_pton(AF_INET, ip, &(addr.sin_addr));
	if (tcp_connect_with_timeout(fd, addr, timeout) < 0) {
		close(fd);
		goto exit;
	}
	if ((conn = malloc(sizeof(*conn))) == NULL) {
		ret = -1;
		conn_mgr_conn_errno = CONN_MGR_ERR_MEM_ALLOC;
		close(fd);
		goto exit;
	}
	conn_mgr_conn_errno = CONN_MGR_SUCCESS;
	conn->fd = fd;
exit:
	return conn;
}
/**@brief destroy a conn object returned by conn_mgr_conn_new
 * @param[in] conn Pointer to a conn object
 * */
void conn_mgr_conn_destroy(conn_mgr_conn_t *conn)
{
	if (conn == NULL) {
		goto exit;
	}
	close(conn->fd);
	free(conn);
	conn_mgr_conn_errno = CONN_MGR_SUCCESS;
exit:
	return;
}

/**@brief Get fd associated with the conn object
 * @param[in] conn Pointer to a conn object
 * @return On success, an int value for the fd.
 *	   On failure, -1
 * */
int conn_mgr_conn_get_fd(conn_mgr_conn_t *conn) 
{
	int ret = 0;
	if (conn == NULL) {
		conn_mgr_conn_errno = CONN_MGR_ERR_NULL_PARAMS;
		ret = -1;
		goto exit;
	}
	ret = conn->fd;
	conn_mgr_conn_errno = CONN_MGR_SUCCESS;
exit:
	return ret;
}

static int tcp_connect_with_timeout(int fd, struct sockaddr_in addr, 
								int timeout)
{
	int ret = 0;

	if (set_socket_non_blocking(fd) < 0) {
		ret = -1;
		conn_mgr_conn_errno = CONN_MGR_CONN_ERR_SETNONBLOCKING;
		goto exit;
	}
	if (connect(fd, (struct sockaddr *)&addr, 
				(socklen_t)sizeof(addr)) < 0) {
		if (errno == EINPROGRESS) {
			ret = poll_for_connect(fd, timeout);	
		} else {
			conn_mgr_conn_errno = CONN_MGR_CONN_ERR_CONNECT;
			goto exit;
		}
	}
	if (set_socket_blocking(fd) < 0) {
		ret = -1;
		conn_mgr_conn_errno = CONN_MGR_CONN_ERR_SETBLOCKING;
		goto exit;
	}
exit:
	return ret;
}

static int poll_for_connect(int fd, int timeout)
{
	struct pollfd pfd;
	int ret = 0;
	pfd.fd = fd;
	pfd.events = POLLOUT;
	int optval = 0;
	socklen_t len = 0;

	while (1) {
		ret = poll(&pfd, 1, timeout);
		if (ret < 0) {
			if (errno == EINTR) {
				continue;
			} else {
				conn_mgr_conn_errno = CONN_MGR_CONN_ERR_POLL;
				break;
			}
		} else if (ret == 0) {
			ret = -1;
			conn_mgr_conn_errno = CONN_MGR_CONN_ERR_TIMEDOUT;
			break;
		} else {
			if (pfd.revents & POLLOUT) {
				break;
			} else {
				ret = -1;
				getsockopt(fd, SOL_SOCKET, SO_ERROR,
						(void *)&optval, &len);
				conn_mgr_conn_errno = 
					CONN_MGR_CONN_ERR_CONNECTING;
				break;
			}
		}
	}
	return ret;
}

static int set_socket_non_blocking(int fd)
{
	int ret = 0;
	int flags = 0;

	if ((flags = fcntl(fd, F_GETFL, 0)) < 0) {
		ret = -1;
		goto exit;
	}
	flags |= O_NONBLOCK;
	if ((ret = fcntl(fd, F_SETFL, flags)) < 0) {
		ret = -1;
		goto exit;
	}
exit:
	return ret;
}

static int set_socket_blocking(int fd)
{
	int ret = 0;
	int flags = 0;

	if ((flags = fcntl(fd, F_GETFL, 0)) < 0) {
		ret = -1;
		goto exit;
	}
	flags &= (~O_NONBLOCK);
	if ((ret = fcntl(fd, F_SETFL, flags)) < 0) {
		ret = -1;
		goto exit;
	}
exit:
	return ret;
}
