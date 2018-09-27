#include <stdio.h>
#include <time.h>

#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)

void test()
{
	int a = 0;
	getchar();
	for (int i = 0; i < 1000000000; i++) {
			a++; a++;
	}
}
int main()
{
	clock_t t, s, e;
	s = clock();
	test();
	e = clock();
	t = e - s; 
	double time_taken = ((double)t)/CLOCKS_PER_SEC;

	printf("%ld %ld %f", s, e, time_taken);
	return 0;
}
