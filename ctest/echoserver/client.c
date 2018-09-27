/*
 *   @filename : client.c
 *     @brief: Client Program
 *     */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_LINE  256


void *SendReceive(void* sockid);

int main(int argc,char *argv[])
{
  struct sockaddr_in  serv_addr ;
  int           sockid,  tcp_port ;
  pthread_t     th;

  if(argc < 3 )
  {
    printf("usage: %s <server-address> <port> \n",argv[0]);
    return -1;
  }
  tcp_port = atoi(argv[2]) ;

  sockid = socket(AF_INET,SOCK_STREAM,0) ;
  if(sockid == -1 )
  {
    printf("Failed to get socket\n");
    return -2;
  }

  serv_addr.sin_family = AF_INET ;
  serv_addr.sin_addr.s_addr = inet_addr(argv[1]) ;
  serv_addr.sin_port = htons(tcp_port) ;

  if(connect(sockid,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) != 0)
  {
    printf("Failed to Connect Server %s\n",argv[2]);
    return -3 ;
  }

  if (pthread_create (&th, NULL, SendReceive, (void*)sockid) != 0)
  {
    close(sockid) ;
    return -4 ;
  }

  pthread_join(th,NULL);
  close(sockid);

  return 0 ;
}

void *SendReceive(void* sockfd)
{

  char  buf[MAX_LINE],sendbuf[MAX_LINE];
  int   length,ret,sockid ;

  sockid =(int)sockfd;
  while(1)
  {
    memset(buf,0,sizeof(buf));
    if(fgets(buf,MAX_LINE,stdin) == NULL )
    {
      printf ("received EOF from stdin\n");
      break;
    }

    length = strlen(buf);

    if( (ret = send(sockid, &buf, length, 0)) <= 0 )
    {
      printf("Client coultn't send message to server\n");
      break;
    }

    ret = recv(sockid,&buf,MAX_LINE,0);
    if ( ret == 0 )
    {
      break;
    }
    else if ( ret < 0 )
    {
      printf("Error Occurs Err[#%d,%s]\n", errno, strerror(errno) );
      break;
    }
    printf("%s",buf);
  }

  return NULL;
}


