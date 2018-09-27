/*
 * This file contains proxy poller implementaion using epoll
 */
/*System Headers*/
#include<errno.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include<sys/epoll.h>
/*Local Headers*/
#include<proxy_poller.h>
#include<proxy_log.h>
#include<proxy_core.h>
#include<proxy_signal_handler.h>
/*Common Headers*/

static int create_poller_thread(struct poller_info *poller_info);
static int handle_epoll_events(struct epoll_event *events, int num_events,
		                                              WorkQT *workq);

int init_poller(struct poller_info *poller_info)
{
	int ret = 0;

	if (poller_info == NULL)  {
		log_msg(LOG_ERR, "NULL params received !!!");
		ret = -1;
		goto exit;
	}
	if ((poller_info->epoll_fd = epoll_create1(0)) < 0) {
		log_msg(LOG_ERR, "Error [%d] [%s] in epoll_create1");
		ret = -1;
		goto exit;
	}
	poller_info->events = calloc(EPOLL_MAX_EVENTS, 
					sizeof(struct epoll_event));
	if (poller_info->events == NULL) {
		log_msg(LOG_CRIT, "Error in calloc");
		ret = -1;
		close(poller_info->epoll_fd);
		goto exit;

	}
	if (create_poller_thread(poller_info) < 0) {
		close(poller_info->epoll_fd);
		free(poller_info->events); poller_info->events = NULL;
		log_msg(LOG_ERR, "Error in create_poller_thread");
		ret = -1;
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
	if ((ret = pthread_cancel(poller_info->poller_id)) != 0) {
		log_msg(LOG_ERR, "Error [%d] [%s] in phread_cancel",
							ret, strerror(ret));
		ret = -1;
		goto exit;
	}
	if ((ret = pthread_join(poller_info->poller_id, NULL)) != 0) {
		log_msg(LOG_ERR, "Error [%d] [%s] in phread_join",
							ret, strerror(ret));
		ret = -1;
	}
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
	}
exit :
	return ret;
}

void *proxy_default_poller(void *arg)
{
	struct poller_info *poller_info = NULL;
	int ret = 0;
	
	if (arg == NULL) {
		log_msg(LOG_ERR, "NULL params received");
		goto exit;
	}
	poller_info = arg;
	while (1) {
		ret = epoll_wait(poller_info->epoll_fd, 
					poller_info->events, 
						EPOLL_MAX_EVENTS, -1);
		if (ret < 0) {
			if (errno == EINTR) {
				continue;
			} else {
				log_msg(LOG_ALERT, "Error [%d] [%s] in"
						"epoll_wait", errno,
							strerror(errno));
				goto exit;
			}
		
		} else {
			if (handle_epoll_events(poller_info->events, ret,
						poller_info->workq) < 0) {
				log_msg(LOG_ERR, "Error in handle_epoll_event");
			}

		}
	}
exit:
	_exit(0);
}

static int handle_epoll_events(struct epoll_event *events, int num_events, 
								WorkQT *workq)
{
	int ret = 0;
	int i = 0;
	struct proxy_info *proxy_info = NULL;
	if (events == NULL || workq == NULL) {
		log_msg(LOG_ERR, "NULL params received");
		ret = -1;
		goto exit;
	}
	log_msg(LOG_INFO, "Events on [%d] fds", num_events);
	for (i = 0; i < num_events; i++) {
		proxy_info = events[i].data.ptr;
		if (events[i].events & EPOLLERR ||
			events[i].events & EPOLLHUP ) {
			log_msg(LOG_ERR, "Error occured on fd",
						proxy_info->client_sock);
			close(proxy_info->client_sock);
			if (free_proxy_info(&proxy_info) < 0) {
				log_msg(LOG_ERR, "Error in free_proxy_info");
			}
		} else {
			log_msg(LOG_DEBUG,"Added fd [%d] to workq", 
							proxy_info->client_sock);
			if (WorkQAdd(workq, proxy_info) != 0) {
				log_msg(LOG_ALERT, "Error in WorkQAdd");
			}
		}
	}
exit :
	return ret;
}

static int create_poller_thread(struct poller_info *poller_info) 
{
	int ret = 0;

	if (poller_info == NULL)  {
		log_msg(LOG_ERR, "NULL params received");
		ret = -1;
		goto exit;
	}
	if (poller_info->poller_routine == NULL) {
		poller_info->poller_routine = proxy_default_poller;
	}
	if (pthread_create(&poller_info->poller_id, NULL, 
					poller_info->poller_routine,
					(void *)poller_info) < 0 ) {
		log_msg(LOG_ERR, "Error in pthread_create ");
		ret = -1;
	}
exit :
	return ret;
}
