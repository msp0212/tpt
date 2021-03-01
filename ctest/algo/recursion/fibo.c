#include <stdio.h>
#include <stdlib.h>

int fibo(int n)
{

	if (n == 0 || n == 1) {
		return n;
	} else {
	       return fibo(n-2) + fibo(n-1);	
	}
}

int main(int argc, char *argv[]) 
{
	int i;

	for (i = 0; i < atoi(argv[1]); i++) {
		printf("%d\n",fibo(i));
	}
	return 0;
}
