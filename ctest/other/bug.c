#include<stdio.h>

int array[] = {1,2,3,4,5};

#define  SIZE sizeof(array)
int main()
{
	int i = -1;

	printf("i = %d and SIZE = %d\n",i,SIZE);

	if(i <= SIZE)
	{
		printf("TRUE\n");
	}
	else
	{
		printf("FALSE\n");
	}
	return 0;
}
