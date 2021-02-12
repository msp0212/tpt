/*
 * This file poller implementaion using epoll
 */
/*System Headers*/
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/epoll.h>
/*Local Headers*/
#include <poller.h>
#include <log.h>

int init_poller(struct poller_info **poller_info)
{
	int ret = 0;

	if (poller_info == NULL)  {
		log_msg(LOG_ERR, "NULL params received !!!");
		ret = -1;
		goto exit;
	}
	*poller_info = malloc(sizeof(**poller_info));
	if (*poller_info == NULL) {
		log_msg(LOG_ERR, "Error in malloc");
		ret = -1;
		goto exit;
	}
	if (((*poller_info)->epoll_fd = epoll_create1(0)) < 0) {
		log_msg(LOG_ERR, "Error [%d] [%s] in epoll_create1");
		ret = -2;
		goto exit;
	}
	(*poller_info)->event_size = EPOLL_MAX_EVENTS;
	(*poller_info)->events = calloc((*poller_info)->event_size, 
					sizeof(struct epoll_event));
	if ((*poller_info)->events == NULL) {
		log_msg(LOG_CRIT, "Error in calloc");
		ret = -3;
		close((*poller_info)->epoll_fd);
		goto exit;

	}
exit :
	return ret;
}

int destroy_poller(struct poller_info *poller_info)
{
	int ret = 0;
	if (poller_info == NULL) {
		log_msg(LOG_ERR, "NULL params received");
		ret = -1;
		goto exit;
	}
	close(poller_info->epoll_fd);
	free(poller_info->events);poller_info->events = NULL;
exit :
	return ret;
}

int add_to_poller(int epoll_fd, int fd, enum event_flag event_flag, void *data)
{
	int ret = 0;
	struct epoll_event event;

	event.data.ptr = data;
	switch (event_flag) {
	case EVENT_IN :
		event.events = EPOLLIN | EPOLLPRI | EPOLLET;
		break;
	case EVENT_OUT :
		event.events = EPOLLOUT | EPOLLET;
		break;
	case EVENT_INOUT :
		event.events = EPOLLIN | EPOLLPRI | EPOLLOUT | EPOLLET;
		break;
	default :
		ret = -1;
		log_msg(LOG_ERR, "Invalid event_flag [%d]", event_flag);
		goto exit;
	}
	ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
	if (ret < 0) {
		log_msg(LOG_ERR, "Error [%d] [%s] in epoll_ctl", 
						errno, strerror(errno));
		ret = -2;
	}
exit :
	return ret;
}
