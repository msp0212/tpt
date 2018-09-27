/*
 * @filename : server.c
 * @brief:  Concurrent  Server Program
 * */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>


#define MAX_LINE 256

pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
int         conn_count=0;
void *HandlingClient(void *);

int main(int argc,char *argv[])
{
  struct sockaddr_in  serv_addr,  client_addr;
  int           sockid, newsockid, tcp_port, len;
  pthread_t     tid;

  if(argc < 2 )
  {
    printf("usage: %s <port>\n", argv[0]);
    return -1;
  }
  tcp_port = atoi(argv[1]);

  sockid = socket(AF_INET,SOCK_STREAM,0);
  if(sockid < 0 )
  {
    printf("Failed to get socket\n");
    return -1;
  }
  len=1;
  setsockopt (sockid, SOL_SOCKET, SO_REUSEADDR, &len, sizeof(int));

  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(tcp_port);
  if((bind(sockid,(struct sockaddr*)&serv_addr,sizeof(serv_addr))) != 0)
  {
    printf("Failed to bind\n");
    return -2;
  }

  pthread_mutex_init(&mutex, NULL);

  listen(sockid,128);

  while (1)
  {
    printf("active connections #%d\n", conn_count);

    len = sizeof(client_addr);
    if((newsockid = accept(sockid,(struct sockaddr*)&client_addr,&len)) == -1)
    {
      printf("Failed to accept the client request\n");
      continue;
    }

    if(pthread_create(&tid, NULL, HandlingClient, (void*)newsockid) != 0 )
    {
      printf("Failed to create thread\n");
      close (newsockid);
      sleep (1);
      continue;
    }

    pthread_mutex_lock(&mutex);
    conn_count ++;
    pthread_mutex_unlock(&mutex);
  }

  pthread_mutex_destroy(&mutex);
  return 0;
}
void *HandlingClient(void *id)
{
  char buf[MAX_LINE];
  int ret, sockid;

  sockid = (int)id;


  while(1)
  {

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

    if(!strncasecmp(buf,"bye",3))
    {
      pthread_mutex_lock(&mutex);
      conn_count--;
      pthread_mutex_unlock(&mutex);
      break;
    }

    /* send back to client*/
    if( (ret = send(sockid, &buf, ret, 0)) <= 0 )
    {
      printf("Client coultn't send message to server\n");
      break;
    }
  }

  close(sockid);

  return NULL;
}

