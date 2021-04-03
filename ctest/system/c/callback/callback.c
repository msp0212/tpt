#include<stdio.h>
#include<unistd.h>
#include "reg_callback.h"

int my_callback(char *msg)
{
	printf("%s\n",msg);
	sleep(3);
	return 0;
}

int main()
{
	//assign address of my callback to func ptr
	callback ptr_my_callback = my_callback;

	printf("Inside Main... \n");

	//register our callback function
	reg_callback(ptr_my_callback);

	printf("Returned to Main...\n");
	return 0;
}

