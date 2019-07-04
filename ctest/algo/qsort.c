#include <stdio.h>

void qsort(int *arr, int left, int right);
void swap(int *arr, int  left, int right);
void print_arr(int *arr, int len);

int main()
{
	int arr[] = {7, 2, 6, 1, 11, 4, 9, 15, 3};
	
	print_arr(arr, sizeof(arr) / sizeof(arr[0]));
	swap(arr, 0, 1);
	print_arr(arr, sizeof(arr) / sizeof(arr[0]));
	qsort(arr, 0, (sizeof(arr) / sizeof(arr[0])) - 1);
	print_arr(arr, sizeof(arr) / sizeof(arr[0]));
	return 0;

}

void qsort(int *arr, int left, int right)
{
	int i;
	int last;

	/*If number of elements in array are less than 2, then do nothing*/
	if (left >= right) 
		return;

	/*choose middle element as pivot*/
	swap(arr, left, (left + right) / 2);
	last = left;

	/* Partition the array: move all elements less than pivot to its left 
	 * and greater to its right*/
	for (i = left + 1; i <= right; i++) {
		if (arr[i] < arr[left]) {
			swap(arr, ++last, i);
		}
	}
	swap(arr, left, last);

	/*recursively sort left and right subarrays*/
	qsort(arr, left, last - 1);
	qsort(arr, last + 1, right);
}

void swap(int *arr, int left, int right)
{
#if 1 
	arr[left] = arr[left] ^ arr[right];
	arr[right] = arr[left] ^ arr[right];
	arr[left] = arr[left] ^ arr[right];
#else
	int tmp;
	tmp = arr[right];
	arr[right] = arr[left];
	arr[left] = tmp;
#endif
}

void print_arr(int *arr, int len)
{
	int i;

	for(i = 0; i < len; i++) {
		printf("%d ", arr[i]);
	}
	printf("\n");
}
