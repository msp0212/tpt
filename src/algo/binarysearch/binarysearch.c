#include <stdio.h>


int cnt = 0;

int binary_search(int *arr, int beg, int end, int key)
{
    int mid;

    mid = (beg + end) / 2;
    printf("binary_search: %d-%d, %d\n", beg, end, mid);

    cnt++;

    if (beg > end) {
        printf("Not found in %d iterations\n", cnt);
        return -1;
    }
        
    if (arr[mid] == key) { /*hit*/
        printf("found in %d iterations\n", cnt);
        return mid;
    } else if (arr[mid] > key) { /*key is in the lower half*/
        return binary_search(arr, beg, mid - 1, key);
    } else { /*key is in the upper half*/
        return binary_search(arr, mid + 1, end, key);
    }
}

void do_binary_search(int *arr, int beg, int end, int find_num)
{
    int i;

    printf("\ninput array = ");
    for (i = beg; i < end; i++) {
        printf("%d ", arr[i]);
    }
    printf("\nnum to be found %d\n", find_num);
    
    int inx = binary_search(arr, beg, end - 1, find_num);
    if (inx < 0) {
        printf("Not found\n");
    } else {
        printf("Found element at inx %d\n", inx);
    }
    cnt = 0;
}

void test1(void)
{
    int arr[] = {1, 2, 4, 8, 9, 12, 21};

    do_binary_search(arr, 0, sizeof(arr) / sizeof(arr[0]), 8);
}

void test2(void)
{
    int arr[] = {1, 2, 4, 8, 9, 12, 21};

    do_binary_search(arr, 0, sizeof(arr) / sizeof(arr[0]), 1);
}

void test3(void)
{
    int arr[] = {1, 2, 4, 8, 9, 12, 21};

    do_binary_search(arr, 0, sizeof(arr) / sizeof(arr[0]), 21);
}

void test4(void)
{
    int arr[] = {1, 2, 4, 8, 9, 12, 21};

    do_binary_search(arr, 0, sizeof(arr) / sizeof(arr[0]), 0);
}

void test5(void)
{
    int arr[] = {1, 2, 4, 8, 9, 12, 21};

    do_binary_search(arr, 0, sizeof(arr) / sizeof(arr[0]), 35);
}

void test6(void)
{
    int arr[] = {1};

    do_binary_search(arr, 0, sizeof(arr) / sizeof(arr[0]), 1);
}

void test7(void)
{
    int arr[] = {1};

    do_binary_search(arr, 0, sizeof(arr) / sizeof(arr[0]), 0);
}

void test8(void)
{
    int arr[] = {1};

    do_binary_search(arr, 0, sizeof(arr) / sizeof(arr[0]), 2);
}

void test9(void)
{
    int arr[] = {1, 2, 4, 8, 9, 12, 21};

    do_binary_search(arr, 0, sizeof(arr) / sizeof(arr[0]), 4);
}

void test10(void)
{
    int arr[] = {1, 2, 4, 8, 9, 12, 21};

    do_binary_search(arr, 0, sizeof(arr) / sizeof(arr[0]), 12);
}

int main(int argc, char *argv[])
{
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    test7();
    test8();
    test9();
    test10();
    return 0;
}

