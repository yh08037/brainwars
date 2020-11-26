#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

typedef struct _node {
    void *data;
    struct _node* next;
} node;

typedef struct _queue {
    node* front;
    node* rear;
    int sz;
} queue;

void queue_init(queue *q);
int queue_size(queue *q);
bool queue_empty(queue *q);
void queue_enqueue(queue *q, void *val);
void queue_dequeue(queue *q);
void *queue_front(queue *q);

#endif // QUEUE_H