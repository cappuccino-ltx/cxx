
#include "queue.h"

// typedef BTNode* q_type;

// typedef struct queue{
//     s_type* _a;
//     int _capacity;
//     int _index;
// }queue;

queue* init_queue(int capacity) {
    queue* q = (queue*)malloc(sizeof(queue));
    q->_capacity = capacity;
    q->_front = -1;
    q->_tail = -1;
    q->_a = (q_type*)malloc(sizeof(q_type) * q->_capacity);
    return q;
} 


void q_push(queue* q, q_type v) {
    assert(q);
    if ((q->_tail + 1) % q->_capacity == q->_front) {
        return ;
    }
    if (q->_front == -1){
        q->_front += 1;
        q->_tail += 2;
        q->_a[q->_front] = v;
    }
    else {
        q->_a[q->_tail] = v;
        q->_tail = (q->_tail + 1) % q->_capacity;
    }
    
}
q_type q_front(queue* q) {
    assert(q);
    return q->_a[q->_front];
}
void q_pop(queue* q) {
    assert(q);
    if (q->_front + 1 == q->_tail){
        q->_front = q->_tail = -1;
    }
    else {
        q->_front += 1;
        q->_front %= q->_capacity; 
    }
}
bool q_empty(queue* q) {
    return q->_front == -1;
}

void q_destory(queue** qq) {
    assert(qq);
    if (*qq){
        if ((*qq)->_a)
            free((*qq)->_a);
        free(*qq);
        *qq = NULL;
    }
}
