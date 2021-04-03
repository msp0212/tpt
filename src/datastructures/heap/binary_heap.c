#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/*min heap implementation*/

struct binary_heap {
    int *elems;
    int elem_cnt;
    int size;
};

struct binary_heap *binary_heap_create(int size);
void binary_heap_destory(struct binary_heap **heap);
void binary_heap_print(struct binary_heap *heap);
/*simpy add an element to binary heap without fixing the heap property*/
void binary_heap_add(struct binary_heap *heap, int elem);
/*add an element to a binary heap and fix the heap property*/
void binary_heap_add_and_fix_min(struct binary_heap *heap, int elem);
/* fix min heap at fix_inx and assumes subtrees of fix_inx are already fixed*/
void binary_heap_fix_min(struct binary_heap *heap, int fix_inx);
/*min heapify a binary heap*/
void binary_heap_min_heapify(struct binary_heap *heap, int fix_inx);
void binary_heap_remove_min(struct binary_heap *heap, int *elem);

void test1(void);
void test2(void);

static inline int right_child_index(int elem_inx);
static inline int left_child_index(int elem_inx);
static inline int parent_index(int elem_inx);
static inline void swap_array_values(int *arr, int inx1, int inx2);

static inline int right_child_index(int elem_inx)
{
    return (2 * elem_inx + 2);
}

static inline int left_child_index(int elem_inx)
{
    return (2 * elem_inx + 1);
}

static inline void swap_array_values(int *arr, int inx1, int inx2)
{
    int tmp;

    tmp = arr[inx1];
    arr[inx1] = arr[inx2];
    arr[inx2] = tmp;
}

static inline int parent_index(int elem_inx)
{
    return (elem_inx - 1) / 2;
}

struct binary_heap *binary_heap_create(int size)
{
    struct binary_heap *heap;

    heap = malloc(sizeof(*heap));
    assert(heap);

    heap->elems = malloc(sizeof(*heap->elems) * size);
    assert(heap->elems);

    heap->elem_cnt = 0;
    heap->size = size;

    return heap;
}

void binary_heap_destory(struct binary_heap **heap)
{
    if ((*heap)->elems) {
        free((*heap)->elems);
        (*heap)->elems = NULL;
    }

    free(*heap);
    *heap = NULL;

    return;
}

void binary_heap_add(struct binary_heap *heap, int elem)
{
    assert(heap->elem_cnt < heap->size);

    /*insert element at the end*/
    heap->elems[heap->elem_cnt++] = elem;
    return;
}

void binary_heap_add_and_fix_min(struct binary_heap *heap, int elem)
{
    int iter;
    int parent_inx;

    assert(heap->elem_cnt < heap->size);

    /*insert element at the end*/
    iter = heap->elem_cnt++;
    heap->elems[iter] = elem;

    /*fix the heap property if it is violated*/
    while (iter > 0) {
        parent_inx = parent_index(iter);
        if (heap->elems[parent_inx] > heap->elems[iter]) {
            swap_array_values(heap->elems, parent_inx, iter);
        }
        iter = parent_inx;
    }

    return;
}

void binary_heap_fix_min(struct binary_heap *heap, int fix_inx)
{
    int left_child_inx;
    int right_child_inx;
    int smallest_inx;
    

    left_child_inx = left_child_index(fix_inx);
    right_child_inx = right_child_index(fix_inx);

    smallest_inx = fix_inx;

    if (left_child_inx < heap->elem_cnt &&
            heap->elems[left_child_inx] < heap->elems[smallest_inx]) {
        smallest_inx = left_child_inx;
    }
    if (right_child_inx < heap->elem_cnt &&
            heap->elems[right_child_inx] < heap->elems[smallest_inx]) {
        smallest_inx = right_child_inx;
    }

    if (smallest_inx != fix_inx) {
        swap_array_values(heap->elems, fix_inx, smallest_inx);
        binary_heap_fix_min(heap, smallest_inx);
    }
    return; 
}

void binary_heap_remove_min(struct binary_heap *heap, int *elem)
{
    /*min heap: element at inx 0 is the smallest*/
    *elem = heap->elems[0];

    /*move last element to inx 0*/
    heap->elems[0] = heap->elems[heap->elem_cnt - 1];
    heap->elem_cnt--;

    /*fix the heap property*/
    binary_heap_fix_min(heap, 0);

    return;
}

void binary_heap_min_heapify(struct binary_heap *heap, int fix_inx)
{
    int left_child_inx;
    int right_child_inx;
    int smallest_inx;

    if (fix_inx >= heap->elem_cnt) return;

    left_child_inx = left_child_index(fix_inx);
    right_child_inx = right_child_index(fix_inx);

    smallest_inx = fix_inx;

    if (left_child_inx < heap->elem_cnt &&
            heap->elems[left_child_inx] < heap->elems[smallest_inx]) {
        smallest_inx = left_child_inx;
    }
    if (right_child_inx < heap->elem_cnt &&
            heap->elems[right_child_inx] < heap->elems[smallest_inx]) {
        smallest_inx = right_child_inx;
    }

    if (smallest_inx != fix_inx) {
        swap_array_values(heap->elems, fix_inx, smallest_inx);
    }
    binary_heap_fix_min(heap, left_child_inx);
    binary_heap_fix_min(heap, right_child_inx);
    return; 
}

void binary_heap_print(struct binary_heap *heap)
{
    int i;

    printf("heap elements: ");
    for (i = 0; i < heap->elem_cnt; i++) {
        printf("%d ", heap->elems[i]);
    }
    printf("\n");
}

void test1(void)
{
    struct binary_heap *heap;
    int elem;
    int i;

    printf("%s\n", __func__);
    heap = binary_heap_create(64);
    
    binary_heap_add_and_fix_min(heap, 7);
    binary_heap_add_and_fix_min(heap, 1);
    binary_heap_add_and_fix_min(heap, 8);
    binary_heap_add_and_fix_min(heap, 3);
    binary_heap_add_and_fix_min(heap, 4);
    binary_heap_add_and_fix_min(heap, 2);
    binary_heap_add_and_fix_min(heap, 5);
    binary_heap_add_and_fix_min(heap, 6);
    binary_heap_add_and_fix_min(heap, 9);

    binary_heap_print(heap);   

    for (i = 0; i < 9; i++) {
        binary_heap_remove_min(heap, &elem);
        printf("%d\n", elem);
    }

    binary_heap_destory(&heap);
    return;
}
void test2(void)
{
    struct binary_heap *heap;
    int elem;
    int i;

    printf("%s\n", __func__);
    heap = binary_heap_create(64);
    
    binary_heap_add(heap, 7);
    binary_heap_add(heap, 1);
    binary_heap_add(heap, 8);
    binary_heap_add(heap, 3);
    binary_heap_add(heap, 4);
    binary_heap_add(heap, 2);
    binary_heap_add(heap, 5);
    binary_heap_add(heap, 6);
    binary_heap_add(heap, 9);

    binary_heap_print(heap); 

    binary_heap_min_heapify(heap, 0);

    binary_heap_print(heap); 

    for (i = 0; i < 9; i++) {
        binary_heap_remove_min(heap, &elem);
        printf("%d\n", elem);
    }

    binary_heap_destory(&heap);
    return;
}

int main(void)
{
    test1();    
    test2();    
    return 0;
}
