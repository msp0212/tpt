#include<stdio.h>
#include<pthread.h>

int main()
{
	printf("char = %d\n", sizeof(char));
	printf("int = %d\n", sizeof(int));
	printf("long = %d\n", sizeof(long));
	printf("long long = %d\n", sizeof(long long));
	printf("float = %d\n", sizeof(float));
	printf("double = %d\n", sizeof(double));
	printf("pthread_mutex_t = %d\n", sizeof(pthread_mutex_t));
	return 0;
}
