#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<netinet/in.h>

int main(int argc , char *argv[])
{
	struct addrinfo hints,*res,*p; //res is a pointer to linked of results returned by getaddrinfo()
	int status;
	char ipstr[INET6_ADDRSTRLEN];  
	if(argc!=2)
	{
		printf("Usage: showip hostname \n");
		return 1;
	}

	memset(&hints,0,sizeof hints); //set struct to zero initially
	hints.ai_family=AF_UNSPEC;     //family unspecified
	hints.ai_socktype=SOCK_STREAM; //tcp
	if((status=getaddrinfo(argv[1],NULL,&hints,&res))!=0)
	{
		fprintf(stderr,"getaddrinfo:%s\n",gai_strerror(status));
		return 2;
	}

	printf("IP Addresses for %s \n\n",argv[1]);

	for(p=res;p!=NULL;p=p->ai_next)  //traverse the linked list of results
	{
		void *addr;  
		char *ipver;
		if(p->ai_family==AF_INET) //ipv4
		{
			struct sockaddr_in *ipv4 =(struct sockaddr_in *)p->ai_addr;
			addr=&(ipv4->sin_addr);
			ipver="IPv4";
		}
		else
		{//ipv6
			struct sockaddr_in6 *ipv6 =(struct sockaddr_in6 *)p->ai_addr;
			addr=&(ipv6->sin6_addr);
			ipver="IPv6";
		}
		inet_ntop(p->ai_family,addr,ipstr,sizeof ipstr);
		printf("%s:%s\n",ipver,ipstr);
	}
	freeaddrinfo(res); //free the linked list
	return 0;
}
