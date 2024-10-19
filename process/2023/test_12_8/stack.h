#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>


typedef int s_type;

typedef struct stack{
    s_type* _a;
    int _capacity;
    int _index;
}stack;

stack* init_stack(int capacity);
void s_push(stack* s, s_type v);
s_type s_top(stack* s);
void s_pop(stack* s);
bool s_empty(stack* s);
void s_destory(stack** sp);
