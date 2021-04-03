#include <stdio.h>
#include <poll.h>
#include <string.h>

int main(int argc, char *argv[])
{
		char buf[10];
		struct pollfd ufds[1];      
		ufds[0].fd = 0;
		ufds[0].events = POLLIN;
		int rv;

		long long_time_out = 5L;

		int int_time_out = 0;

		int_time_out = long_time_out*1000;
		while(1)	
		{
				
				if((rv = poll(ufds, 1, long_time_out*1000)) == -1) 
					perror("poll");
				else if (rv == 0)
				{
					printf("Timeout occurred!\n");
					break;
				}
				else if (ufds[0].revents & POLLIN) 
				{
					scanf("%s",buf);
					if(strcmp(buf,"quit")==0)
						break;
					printf("buf = %s\n",buf);
				}   
				fflush(stdin); 
		}   
		return 0;
}

