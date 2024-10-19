


#include "heap.h"


// typedef int HPDataType;
// typedef struct Heap
// {
// 	HPDataType* _a;
// 	int _size;
// 	int _capacity; 
// }Heap;

void swap(HPDataType* x, HPDataType* y) {
    HPDataType tmp = *x;
    *x = *y;
    *y = tmp;
}

void AdjustUp(HPDataType* a, int size, int child){
    int parent = (child - 1) / 2;
    while (child > 0) {
        if (a[parent] < a[child]){
            //建大堆使用 < ,建小堆使用 > 
            swap(a + parent, a + child);
            child = parent;
        }
        else {
            break;
        }
        parent = (child - 1) / 2;
    }
}
void AdjustDown(HPDataType* a, int size, int parent){
    int child = parent * 2 + 1;
    while(child < size) {
        //找到孩子节点中较大的
        if (child + 1 < size && a[child + 1] > a[child])
            child++;
            //
        if (a[parent] < a[child]) {
            swap(a + parent, a + child);
            parent = child;
        }
        else{
            break;
        }
        child = parent * 2 + 1;
    }
}

void capacity(Heap* hp) {
    if (hp->_size == hp->_capacity){
        hp->_capacity *= 2;
        hp->_a = realloc(hp->_a,sizeof(HPDataType) * hp->_capacity);
    }
}
void HeapInit(Heap* hp,int capacity){
    assert(hp);
    hp->_capacity = capacity;
    hp->_size = 0;
    hp->_a = malloc(sizeof(HPDataType) * hp->_capacity);
}
// 堆的构建
void HeapCreate(Heap* hp, HPDataType* a, int n) {
    assert(hp && hp->_a);
    for (int i = 0; i < n; i++){
        HeapPush(hp,a[i]);
    }
}
// 堆的销毁
void HeapDestory(Heap* hp) {
    assert(hp && hp->_a);
    free(hp->_a);
    hp->_a = NULL;
    hp->_capacity = 0;
    hp->_size = 0;
}
// 堆的插入
void HeapPush(Heap* hp, HPDataType x) {
    assert(hp && hp->_a);
    //插入数据，先判断空间够不够
    capacity(hp);
    //开始插入
    hp->_a[hp->_size++] = x;
    //向上调整
    AdjustUp(hp->_a,hp->_size,hp->_size - 1);
}
// 堆的删除
void HeapPop(Heap* hp) {
    assert(hp && hp->_a);
    //用堆的最后一个元素覆盖堆顶元素
    hp->_a[0] = hp->_a[--hp->_size];
    AdjustDown(hp->_a, hp->_size, 0);
}
// 取堆顶的数据
HPDataType HeapTop(Heap* hp) {
    assert(hp && hp->_a);
    return hp->_size > 0 ? hp->_a[0] : -1;
}
// 堆的数据个数
int HeapSize(Heap* hp) {
    assert(hp);
    return hp->_size;
}
// 堆的判空
int HeapEmpty(Heap* hp) {
    assert(hp);
    return hp->_size == 0;
}