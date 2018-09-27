#include<stdio.h>
#include "reg_callback.h"

//registration goes here
void reg_callback(callback ptr_reg_callback)
{
	int ret;
	printf("Inside register callback\n");

	//calling the callback fucntion
	ret = (*ptr_reg_callback)("Hi this is a callback message !!!");
	printf("ret = %d",ret);
}
