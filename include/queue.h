#ifndef QUEUE_H
#define QUEUE_H

typedef struct qnode {
    const void *val;
    int prio;
    struct qnode *next;
    struct qnode *prev;
} QNode;

typedef struct {
    int len;
    QNode *head;
    QNode *tail;
} Queue;

QNode *create_node(const void *val, int prio);

/* 
 * Appends node to queue based on prio.
 *
 * Effects: Does NOT modify tail. May modify head.
 *
 * @returns 1 if the head has changed. 0 otherwise.
*/
int queue_append(Queue *queue, QNode *node);

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
int queue_remove(Queue *queue, const QNode node);

#endif