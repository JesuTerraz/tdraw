#ifndef QUEUE_H
#define QUEUE_H

typedef struct pnode {
    const void *val;
    int prio;
    struct pnode *next;
} PNode;

/* 
 * A Tiered - Priority Queue.
 *
 * PNodes will be ordered by:
 *  1. The value of prio
 *  2. FIFO after that.
 * 
 * Append: Appends a node based on the above ordering.
 * Remove: Removes a specified node.
*/
typedef struct {
    int len;
    PNode *head;
} PQueue;

PNode *create_node(const void *val, int prio);

/* 
 * Appends node to queue based on prio.
 *
 * Effects: Does NOT modify tail. May modify head.
 *
 * @returns 1 if the head has changed. 0 otherwise.
*/
int queue_append(PQueue *queue, PNode *node);

/* 
 * Removes the first node that matches node.
 *  node.val & node.prio must match.
 *
 * Effects: Handles freeing the removed node. node here is just a
 *  place holder for finding nodes within the queue.
 *  Does NOT modify tail.
 * 
 * @returns 1 if the head has changed. 0 otherwise.
*/
int queue_remove(PQueue *queue, const PNode node);


typedef struct qnode {
    const void *val;
    struct qnode *next;
    struct qnode *prev;
} QNode;

/*
 * A FIFO Queue.
*/
typedef struct {
    int len;
    QNode *head;
    QNode *tail;
} Queue;

/*
 * Appends node to queue as tail.
*/
int queue_push(Queue *queue, QNode *node);

/*
 * Removes the head of the queue.
*/
QNode *queue_pop(Queue *queue);

#endif