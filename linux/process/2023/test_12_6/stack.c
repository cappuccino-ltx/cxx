

#include "stack.h"

stack* init_stack(int capacity) { 
    stack* s = (stack*)malloc(sizeof(stack));
    s->_capacity = capacity;
    s->_index = -1;
    s->_a = (s_type*)malloc(sizeof(s_type) * s->_capacity);
    return s;
}

void capacity(stack* s) {
    if (s->_capacity == s->_index + 1) {
        s->_capacity *= 2;
        s_type* tmp = (s_type*)realloc(s->_a,s->_capacity);
        if (tmp)
            s->_a = tmp;
        else 
            exit(1);
    }
}

void s_push(stack* s, s_type v) {
    assert(s);
    capacity(s);
    s->_a[++(s->_index)] = v;
}

s_type s_top(stack* s){
    return s->_a[s->_index];
}

void s_pop(stack* s) {
    if (s->_index < 0)
        return ;
    (s->_index)--;
}

bool s_empty(stack* s) {
    return s->_index == -1;
}

void s_destory(stack** sp) {
    assert(sp);
    if (*sp){
        if ((*sp)->_a)
            free((*sp)->_a);
        free(*sp);
        *sp = NULL;
    }
}
