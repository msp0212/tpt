#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	FILE *fp = NULL;
	int i = 0;
	char buffer[32]={0,};
	strcpy(buffer,"testing\r\n");
	fp = fopen("lua.txt","a");
	
	if(fp)
	{
		while(i<3)
		{
			printf("writing [%s] to file\n",buffer);
			fwrite(buffer, sizeof(char), strlen(buffer), fp);
			i++;
			sleep(10);
		}
	}
	else
	{
		perror("fopen");
	}
	fclose(fp);
	return 0;
}
