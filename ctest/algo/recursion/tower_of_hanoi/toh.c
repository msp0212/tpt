#include <stdio.h>
#include <stdlib.h>

static int m = 1;

void toh(unsigned int num, unsigned char f, unsigned char t, unsigned a) 
{
	if (num == 0)
		return;

	toh(num - 1, f, a, t);

	printf("move %u: disk %u - peg %u --> peg %u\n", m++, num, f, t);

	toh(num - 1, a, t, f);
}

int main(int argc, char **argv)
{
	unsigned int num;

	if (argc < 2) {
		printf("Wrong no of arguments !!!\n");
		printf("Usage: %s <num>\n", argv[0]);
		exit(1);
	}
	
	num = atoi(argv[1]);
	if (num > 8) {
		printf("Run with num <= 8\n"); 
		exit(1);
	}
	toh(num, 1, 2, 3);

	return 0;
}
