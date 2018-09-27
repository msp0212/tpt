#include<stdio.h>
#include<stdlib.h>

int main(int argc, char **argv)
{
	FILE *fp = NULL;
	char buf[32];
	if(argc != 2)
	{
		printf("Error in passing arguments!!!\nUsage: %s <command>\n",argv[0]);
		exit(1);
	}
	printf("Command = %s\n",argv[1]);
	fp = popen(argv[1],"r");
	if(fp == NULL)
	{
		perror("popen");
		exit(1);
	}
	while(fgets(buf,sizeof(buf),fp) != NULL)
	{
		printf("%s",buf);
	}
	printf("Done\n");
	pclose(fp);
	return 0;
}
