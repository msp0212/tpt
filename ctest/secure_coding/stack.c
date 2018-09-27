#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test(char *s)
{
	char buf[4];
	memcpy(buf, s, strlen(s));
	printf("%s\n", buf);
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		exit(0);
	}
	test(argv[1]);
	return 0;
}
