
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void swap(int* x, int* y) {
    int temp = *x;
    *x = *y;
    *y = temp;
}

void insert_sort(int* arr, int size) {
    for (int i = 0; i < size - 1; i++){
        int end = i;
        int cur = arr[end + 1];
        while(end >= 0) {
            if (arr[end] > cur) {
                arr[end + 1] = arr[end];
                end--;
            }
            else {
                break;
            }
        }
        arr[end + 1] = cur;
    }
}

void share_sort(int* arr, int size) {
    int gap = size;
    while(gap > 1) {
        gap = gap / 3 + 1;
        for (int i = 0; i < size - gap; i += gap) {
            int end = i;
            int cur = arr[end + gap];
            while(end >= 0){
                if (cur < arr[end]) {
                    arr[end + gap] = arr[end];
                    end -= gap;
                }
                else {
                    break;
                }
            }
            arr[end + gap] = cur;
        } 
    }
}

void select_sort(int* arr, int size) {
    for (int i = 0; i < size - 1; i++) {
        int min_index = i;
        for (int j = i + 1; j < size; j++) {
            if (arr[j] < arr[min_index]) {
                min_index = j;
            }
        }
        swap(arr + min_index, arr + i);
    }
}

void adjust_down(int* arr, int size, int parent) {
    int child = parent * 2 + 1;
    while(child < size) {
        if (child + 1 < size && arr[child + 1] > arr[child])
            child++;
        if (arr[child] > arr[parent]){
            swap(arr + child, arr + parent);
        }
        else {
            break;
        }
        parent = child;
        child = parent * 2 + 1;
    }
}

void heap_sort(int* arr, int size) {
    //向下建堆
    for (int i = (size - 1 - 1) / 2; i >= 0; i--) {
        adjust_down(arr,size,i);
    }
    //排序
    for(int i = size - 1; i > 0; i--) {
        swap(arr + i, arr + 0);
        adjust_down(arr,i,0);
    }
}

void bubble_sort(int* arr, int size) {
    for (int i = 0; i < size - 1; i++){
        for (int j = 0; j < size - 1 - i; j++){
            if (arr[j] > arr[j + 1]) {
                swap(arr + j, arr + j + 1);
            }
        }
    }
}

int part_sort(int* arr, int begin, int end) {
    int keyi = begin;
    int prev = begin;
    int cur = begin + 1;
    while(cur <= end){
        if (arr[cur] < arr[keyi] && ++prev != cur){
            swap(arr + cur, arr + prev);
        }
        cur++;
    }
    swap(arr + prev, arr + keyi);
    return prev;
}

void quick_sort(int* arr, int begin, int end) {
    if (begin >= end) {
        return ;
    }
    int keyi = part_sort(arr,begin,end);
    quick_sort(arr,begin,keyi - 1);
    quick_sort(arr,keyi + 1,end);
}

void _merge_sort(int* arr,int* tmp,int begin,int end){
    if (begin >= end) {
        return;
    }
    int mid = (begin + end) / 2;
    _merge_sort(arr,tmp,begin,mid);
    _merge_sort(arr,tmp,mid + 1,end);

    //对两个子数组就行合并
    int begin1 = begin;
    int end1 = mid;
    int begin2 = mid + 1;
    int end2 = end;
    int cur = begin;
    while(begin1 <= end1 && begin2 <= end2) {
        if (arr[begin1] < arr[begin2])
            tmp[cur++] = arr[begin1++];
        else
            tmp[cur++] = arr[begin2++];
    }
    while(begin1 <= end1){
        tmp[cur++] = arr[begin1++];
    }
    while(begin2 <= end2){
        tmp[cur++] = arr[begin2++];
    }
    memcpy(arr + begin, tmp + begin, (end - begin + 1) * sizeof(int));
}

void merge_sort(int* arr, int size) {
    int* tmp = (int *)calloc(size,sizeof(int));
    _merge_sort(arr,tmp,0,size - 1);
    free(tmp);
}

int main() {
    int arr[] = { 4,1,2,5,3,7,9,6,8 };
    int size = sizeof(arr) / sizeof(arr[0]);
    //直接插入排序
    //insert_sort(arr,size);
    //share_sort(arr,size);
    //select_sort(arr,size);
    //heap_sort(arr,size);
    //bubble_sort(arr,size);
    //quick_sort(arr,0,size - 1);
    merge_sort(arr,size);
    for (int i = 0; i < size; i++) {
        printf("%d ",arr[i]);
    }
    printf("\n");
    return 0;
}