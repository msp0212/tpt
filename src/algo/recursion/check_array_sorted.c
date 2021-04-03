#include <stdio.h>

int check_array_sorted(int *arr, int size)
{
	if (size == 1) {
		return 1;
	} else if (*arr < *(arr + 1)) {
		return check_array_sorted(arr + 1, size - 1);
	} else {
		return 0;
	}
}

int main(int argc, char *argv[])
{

	int arr1[] = {1, 2, 3, 4};
	int arr2[] = {1, 2, 6, 4};
	int arr3[] = {3, 2, 4, 6};
	int arr4[] = {1, 2, 4, 6};

	printf("%d\n", check_array_sorted(arr1, 4));
	printf("%d\n", check_array_sorted(arr2, 4));
	printf("%d\n", check_array_sorted(arr3, 4));
	printf("%d\n", check_array_sorted(arr4, 4));
}
