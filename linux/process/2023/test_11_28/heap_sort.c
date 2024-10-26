
#include "heap_sort.h"


void heap_sort(int a[], int size) {
    //建堆
    //向上调整建堆
    // for (int i = 1; i < size; i++) {
    //     AdjustUp(a,size,i);
    // }
    //向下调整建堆
    for (int i = (size - 1 - 1) / 2; i >= 0; i--){
        AdjustDown(a,size,i);
    }


    //排序
    for (int i = size - 1; i > 0; i--){
        swap(a + 0, a + i);
        AdjustDown(a, i, 0);
    }
}