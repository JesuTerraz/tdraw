#include "queue.h"

#include <stdlib.h>

/* 
 * Walk the queue until curr < match.
 *
 * @returns the last node where prio >= match.prio.
 * Returns node if match.prio > node.prio.
*/
static PNode *
walk_queue(PNode *node, PNode *match)
{
    PNode *curr, *prev;

    if (!node || !match) {
        return (0);
    }

    curr = node;
    while (curr->prio >= match->prio) {
        prev = curr;
        curr = curr->next;

        // Found tail.
        if (!curr) {
            return (prev);
        }
    }

    return (prev);
}

static int
compare_nodes(PNode n, PNode m)
{
    return n.prio == m.prio && n.val == m.val;
}


/* PQueue Methods . */

PNode *
create_node(const void *val, int prio)
{
    PNode *node;

    // Should we support negative prios?
    if (!val)
    {
        return (NULL);
    }

    node = malloc(sizeof(PNode));

    if (!node)
    {
        return (NULL);
    }

    node->val = val;
    node->prio = prio;
    node->next = NULL;

    return (node);
}

int
queue_append(PQueue *queue, PNode *node)
{
    // Bad call.
    if (!queue || !node)
    {
        return (0);
    }

    // Adding nodes without values doesn't make sense.
    if (!node->val)
    {
        return (0);
    }

    // We won't support adding a whole queue.
    if (node->next)
    {
        return (0);
    }

    // Queue is empty.
    if (!queue->head) {
        queue->head = node;
        queue->len++;
        return (1);
    }

    // Node fits as first.
    if (queue->head->prio < node->prio) {
        node->next = queue->head;
        queue->head = node;
        queue->len++;
        return (1);
    }

    /*
     * This is a queue so we'll place node at the end of
     * the subqueue where prio = node.prio.
    */
    PNode *prev = walk_queue(queue->head, node);

    // Should not really happen...
    if (!prev) {
        return (0);
    }

    node->next = prev->next;
    prev->next = node;
    queue->len++;
    return (0);
}

int
queue_remove(PQueue *queue, const PNode node)
{
    PNode *removed, *curr, *prev;

    // Bad call
    if (!queue || !node.val)
    {
        return (0);
    }

    // Queue is empty.
    if (!queue->head)
    {
        return (0);
    }

    // Node is head.
    if (compare_nodes(*queue->head, node))
    {
        removed = queue->head;
        queue->head = removed->next;
        queue->len--;

        // Prob shouldn't free val...
        free(removed);
        return (1);
    }

    curr = queue->head;
    while (!compare_nodes(*curr, node)) {
        prev = curr;
        curr = curr->next;

        // Found Tail, this node doesn't exist.
        if (!curr)
        {
            return (0);
        }
    }

    // Only reason we've left while without
    // return is compare_nodes was true.
    prev->next = curr->next;
    queue->len--;

    free(curr);
    return (0);
}

/* Queue Methods. */

QNode *
create_qnode(const void *val)
{
    QNode *node;

    if (!val)
    {
        return (NULL);
    }

    node = malloc(sizeof(QNode));

    if (!node)
    {
        return (NULL);
    }

    node->val = val;
    node->next = node;
    node->prev = node;

    return (node);
}

int
queue_push(Queue *queue, void *val)
{
    QNode *node;

    if (!queue || !val)
        return (-1);

    node = create_qnode(val);
    if (!node)
        return (-1);

    // In case queue is empty. Both should be assigned.
    if (!queue->tail || !queue->head)
    {
        queue->tail = node;
        queue->head = node;
        queue->len++;
        return (0);
    }

    node->prev = queue->tail;
    node->next = queue->head;
    queue->tail->next = node;
    queue->head->prev = node;
    queue->tail = node;

    queue->len++;

    return (0);
}

void *
queue_pop(Queue *queue)
{
    QNode *node;
    void *val;

    if (!queue)
    {
        return (NULL);
    }

    if (!queue->head)
    {
        return (NULL);
    }

    node = queue->head;

    // Queue will be empty.
    if (queue->len == 1)
    {
        queue->head = NULL;
        queue->tail = NULL;
        queue->len = 0;

        val = (void *) node->val;
        free(node);
        return (val);
    }

    queue->head = node->next;
    queue->head->prev = queue->tail;
    queue->tail->next = queue->head;
    queue->len--;

    val = (void *) node->val;
    free(node);

    return (val);
}