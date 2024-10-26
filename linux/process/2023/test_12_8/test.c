

#include <stdio.h>
#include "stack.h"

void swap(int* x, int* y){
    int temp = *x;
    *x = *y;
    *y = temp;
}

int part_sort(int* arr, int begin, int end) {
    int keyi = begin;
    int prev = begin;
    int cur = begin + 1;
    while (cur <= end) {
        if (arr[cur] < arr[keyi] && ++prev != cur)
            swap(arr + cur, arr + prev);
        cur++;
    }
    swap(arr + keyi, arr + prev);
    return prev;
}

void quick_sort(int* arr, int begin, int end){
    stack* s = init_stack(20);
    s_push(s,begin);
    s_push(s,end);
    while(!s_empty(s)) {
        int right = s_top(s);
        s_pop(s);
        int left = s_top(s);
        s_pop(s);
        int keyi = part_sort(arr,left, right);
        if (left < keyi - 1) {
            s_push(s,left);
            s_push(s,keyi - 1);
        }
        if (right > keyi + 1) {
            s_push(s,keyi + 1);
            s_push(s,right);
        }
    }
    s_destory(&s);
}

int main() {
    int arr[] = { 3,1,5,2,8,4,9,6,7 };
    int size = sizeof(arr) / sizeof(arr[0]);
    quick_sort(arr,0,size - 1);
    for (int i = 0; i < size; i++) {
        printf("%d ",arr[i]);
    }
    printf("\n");
    return 0;
}