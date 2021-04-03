#include<stdio.h>

typedef struct obj 
{
	int id;
};

int obj_getSize(void)
{
	return sizeof(struct obj);
}

int obj_setID(struct obj *o, int id)
{
	if(o == NULL)
		return -1;
	o->id = id;
	return 0;
}

int obj_getID(struct obj *o, int *id)
{
	if(o == NULL || id == NULL)
		return -1;
	*id = o->id;
	return 0;
}
