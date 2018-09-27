/*
 * This file contains dns lookup part
 */
/*System Headers*/
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
/*local headers*/
#include<proxy_dns_lookup.h>
#include<proxy_log.h>

int dns_lookup(char *host, char *service, char **ip_address)
{
	int ret = 0;
	struct addrinfo *res = NULL;
	struct addrinfo hints = {0,};

	if (host == NULL || service == NULL || ip_address == NULL) {
		log_msg(LOG_CRIT, "Invalid params received !!!");
		ret = -1;
		goto exit;
	}
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 6; /*TCP*/
	ret = getaddrinfo(host, service, &hints, &res);
	if (ret < 0) {
		log_msg(LOG_ERR, "Error [%d] [%s] in getaddrinfo !!!", ret, gai_strerror(ret));
		ret = -1;
		goto exit;
	}
	*ip_address = strdup( inet_ntoa( ( ( struct sockaddr_in* )res->ai_addr)->sin_addr ) );
	freeaddrinfo(res);
exit :
	return ret;
}
