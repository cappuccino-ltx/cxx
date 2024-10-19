#pragma once 


#include "common.h"


typedef struct queue{
    s_type* _a;
    int _capacity;
    int _front;
    int _tail;
}queue;

queue* init_queue(int capacity);
void q_push(queue* q, q_type v);
q_type q_front(queue* q);
void q_pop(queue* q);
bool q_empty(queue* q);
void q_destory(queue** qq);
