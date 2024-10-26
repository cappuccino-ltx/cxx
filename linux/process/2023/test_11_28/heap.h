#pragma once

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
typedef int HPDataType;
typedef struct Heap
{
	HPDataType* _a;
	int _size;
	int _capacity; 
}Heap;
void swap(HPDataType* x, HPDataType* y);
void AdjustUp(HPDataType* a, int size, int child);
void AdjustDown(HPDataType* a, int size, int parent);
//堆的初始化
void HeapInit(Heap* hp,int capacity);
// 堆的构建
void HeapCreate(Heap* hp, HPDataType* a, int n);
// 堆的销毁
void HeapDestory(Heap* hp);
// 堆的插入
void HeapPush(Heap* hp, HPDataType x);
// 堆的删除
void HeapPop(Heap* hp);
// 取堆顶的数据
HPDataType HeapTop(Heap* hp);
// 堆的数据个数
int HeapSize(Heap* hp);
// 堆的判空
int HeapEmpty(Heap* hp);