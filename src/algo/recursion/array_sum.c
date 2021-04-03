#include <stdio.h>

int array_sum(int *arr, int beg, int end)
{
	if (beg == end) 
		return arr[beg];
	return arr[beg] + array_sum(arr, beg + 1, end);
}

int main()
{
	int arr[] = {1, 2, 3, 4, 5, 6};
	printf("%d\n", array_sum(arr, 0, 5));
	return 0;
}
