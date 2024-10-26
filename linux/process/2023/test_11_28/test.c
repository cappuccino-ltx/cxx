#include "heap.h"
#include "heap_sort.h"
void heapTest() {
    Heap hp;
    HeapInit(&hp,10);
    int arr[] = {4,2,6,5,9,17,13};
    int size = sizeof(arr) / sizeof(arr[0]);
    for (int i = 0; i < size; i++){
        HeapPush(&hp,arr[i]);
    }
    for (int i = 0; i < size; i++){
        printf("%d ",HeapTop(&hp));
        HeapPop(&hp);
    }
    printf("\n");
    HeapDestory(&hp);
}

void heapSortTest(){
    int arr[] = {4,2,6,5,9,17,13};
    int size = sizeof(arr) / sizeof(arr[0]);
    heap_sort(arr,size);
    for (int i = 0; i < size; i++) {
        printf("%d ",arr[i]);
    }
    printf("\n");
}

int main() {
    heapTest();
    //heapSortTest();
    return 0;
}