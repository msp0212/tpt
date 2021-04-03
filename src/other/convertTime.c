#include<stdio.h>
#include<stdlib.h>
#include<time.h>

int main(int argc,char **argv)
{
	time_t unixTS;
	struct tm *myTime;
	if(argc != 2)
	{
		fprintf(stderr,"Error in passing arguments !!! \nUsage : %s <%s>\n", argv[0], "unixTimeStamp");
		exit(1);
	}
	unixTS = atol(argv[1]);

	myTime = localtime(&unixTS);
	printf("%ld -> %d/%d/%d %d:%d:%d\n", unixTS,  myTime->tm_mday, myTime->tm_mon + 1, myTime->tm_year + 1900, myTime->tm_hour, myTime->tm_min, myTime->tm_sec);
	return 0;	
}

