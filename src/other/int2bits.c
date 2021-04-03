#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int shortint2bin(short int a, char **buf); 

int main(void)
{
	short int a = 32768;
	char *buf = NULL;

	printf("%d\n", a);
	shortint2bin(a, &buf);
	//printf("%s\n", buf);
	free(buf); buf = NULL;
	return 0;
}

int shortint2bin(short int a, char **buf)
{
	int i = 0;
	
	*buf = malloc(17 * sizeof(**buf));
	for (i = 15; i >= 0; i--) {
		(*buf)[i] = (a & 1) + '0';
		a >>= 1;
	}
	(*buf)[16] = '\0';
	return 0;
}
