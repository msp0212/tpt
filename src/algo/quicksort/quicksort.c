#include <stdio.h>

void quicksort(int *arr, int low, int high);
int partition(int *arr, int low, int high);
void swap(int *a, int *b);
void print_array(int *arr, int size);
int main()
{
	int arr[10] = {99, 12, 2, 20, 10, 5, 23 ,17 ,9, 15};
	print_array(arr, 10);
	quicksort(arr, 0, 9);
	print_array(arr, 10);
	return 0;
}

void quicksort(int *arr, int low, int high)
{
	int index;

	if (low < high) {
		index = partition(arr, low, high);
		quicksort(arr, low, index - 1);
		quicksort(arr, index + 1, high);
	}
}

int partition(int *arr, int low, int high)
{
	int pivot;
	int store_index;
	int i = 0;
	
	pivot = arr[high];
	store_index = low;

	for (i = low; i < high; i++) {
		if (arr[i] <= pivot) {
			swap(&arr[i], &arr[store_index]);
			store_index++;
		}
	}
	swap(&arr[store_index], &arr[high]);
	return store_index;
}

void swap(int *a, int *b)
{
	int tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

void print_array(int *arr, int size)
{
	int i = 0;
	for (i = 0; i < size; i++) 
		printf("%d ", arr[i]);
	printf("\n");
}
