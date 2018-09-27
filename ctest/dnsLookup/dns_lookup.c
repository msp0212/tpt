#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int main(int argc, char **argv)
{
    char *host = NULL, *service = NULL;
    struct addrinfo *res = NULL, *tmp = NULL, hints= {0,};
    int ret = 0;
    if(argc < 3)
    {
        fprintf(stderr, "%s : Wrong number of arguments to the program\n", __FUNCTION__);
        fprintf(stderr,"Usage: %s <host> <service> \n", argv[0]);
        exit(1); 
    }
    host = strdup(argv[1]);
    service = strdup(argv[2]);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol  = 6;
    ret = getaddrinfo(host, service, &hints, &res);
    if(ret < 0)
    {
        fprintf(stderr, "%s : Error [%d] [%s] in getaddrinfo !!!\n", __FUNCTION__, ret, gai_strerror(ret));
    }
    else
    {
        tmp = res;
        while(tmp != NULL)
        {
            printf("ai_flags [%d] ai_famnily [%d] ai_socktype [%d] ai_protocol [%d] ai_addrlen [%d] ip [%s] port [%d] ai_canonname [%s]"
                    "\n\n", tmp->ai_flags, tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol, tmp->ai_addrlen, 
                     inet_ntoa( ((struct sockaddr_in*)tmp->ai_addr)->sin_addr), ntohs( ((struct sockaddr_in*)tmp->ai_addr)->sin_port) ,
                     tmp->ai_canonname );
            tmp = tmp->ai_next;
        }
    }
    socket(AF_INET, SOCK_STREAM, 0);
    sleep(1000);
    free(host); host = NULL;
    free(service); service = NULL;
    freeaddrinfo(res);
    return 0;
}

