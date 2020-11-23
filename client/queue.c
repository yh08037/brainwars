#include "queue.h"


void queue_init(queue *q) {
    q->front = q->rear = NULL;
    q->sz = 0;
}

int queue_size(queue *q) {
    return q->sz;
}

bool queue_empty(queue *q) {
    return queue_size(q) == 0;
}

void queue_enqueue(queue *q, void *val) {
    node *newNode = (node*)malloc(sizeof(node));
    newNode->data = val;
    newNode->next = 0;
    q->sz++;
    if (q->rear==NULL && q->front==NULL) {
        q->rear=q->front = newNode;
        return;
    }
    q->rear->next = newNode;
    q->rear = newNode;
}

void queue_dequeue(queue *q) {
    assert(!queue_empty(q));
    node *nextHead = q->front->next;
    free(q->front->data);
    free(q->front);
    if (q->front == q->rear) q->front = q->rear = 0;
    q->front = nextHead;
    q->sz--;
}

void *queue_front(queue *q) {
    assert(!queue_empty(q));
    return q->front->data;
}