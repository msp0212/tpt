#include"myHeader.h"
#include<stdio.h>
#include<stdlib.h>

int main()
{
	struct obj *o1 = NULL;
	size_t size;
	int id=99,id1,ret;
	
	size = obj_getSize();
	o1 = (struct obj*)calloc(1,size);
	ret = obj_setID(o1,id);
	if(ret != 0)
	{
		printf("Error in setting obj id !!!\n");
		exit(1);
	}
	ret = obj_getID(o1,&id1);
	if(ret != 0)
    {
        printf("Error in getting obj id !!!\n");
        exit(1);
    }
	printf("id = %d\n",id1);
	return 0;
}
