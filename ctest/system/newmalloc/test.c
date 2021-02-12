#include<stdio.h>
#include<string.h>
#include<gperftools/tcmalloc.h>

int main()
{
	char *myStr = NULL;
	int i;
	for(i=0 ;i<90000000; i++)
	{
	myStr = (char*)tc_malloc(2048*sizeof(char));
	strcpy(myStr, "mohit singh");
//	printf("myStr = [%s]\n",myStr);
	tc_free(myStr);
	}
	return 0;
}
