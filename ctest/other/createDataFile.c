#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>


#define LINE_SIZE (1 * 1024)

int main(int argc, char **argv)
{
    char *file_name = NULL;
    //char *str = "d";
    long file_size = 0L;
    FILE *fp = NULL;
    int j = 0, i = 0;
	int num_lines = 1;

    if(argc < 3)
    {
        fprintf(stderr, "%s : Wrong No of Arguments to the program !!!\n", __FUNCTION__);
        fprintf(stderr, "Usage : %s FILE_NAME FILE_SIZE\n" ,argv[0]);
        return -1;
    }
    file_name = strdup(argv[1]);
    file_size = atol(argv[2]);
    fp = fopen(file_name, "w");
    if(fp == NULL)
    { 
        fprintf(stderr, "%s : Error [%d] [%s] in fopen !!!\n", __FUNCTION__, errno, strerror(errno));
        return -1;
    }
	
	if (file_size > LINE_SIZE)
		num_lines = file_size / LINE_SIZE;

    for(i=0; i < num_lines; i++)
    {   
		for (j = 0; j < LINE_SIZE - 2; j++) {
			fwrite("d", 1, 1, fp);
		}
		fwrite("\r\n", 1, 2, fp);
    }
    fclose(fp);
    free(file_name);
    return -1;
}
