#include<stdio.h>
#include"my_lib.h"

int do_something(char *str)
{
	if (str == NULL) {
		fprintf(stderr, "NULL params received !!!\n");
		return -1;
	}
	printf("TEST123456\nString [%s]\n", str);
	return 0;
}
