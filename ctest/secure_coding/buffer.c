#include <stdio.h>

int main()
{
	int cookie = 0;
	char buf[16];

	printf("Input: ");
	scanf("%s", buf);

	if (cookie == 0x44434241) {
		printf("You found the secret!\n");
	} else {
		printf("Nice try!\n");
	}
	return 0;
}
