#include "queue.h"

#include <stdlib.h>

/* 
 * Walk the queue until curr < match.
 *
 * @returns the last node where prio >= match.prio.
 * Returns node if match.prio > node.prio.
*/
static QNode *
walk_queue(QNode *node, QNode *match)
{
    QNode *curr, *prev;

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
compare_nodes(QNode n, QNode m)
{
    return n.prio == m.prio && n.val == m.val;
}


/* Public facing methods. */

QNode *
create_node(const void *val, int prio)
{
    QNode *node;

    // Should we support negative prios?
    if (!val)
    {
        return (NULL);
    }

    node = malloc(sizeof(QNode));
    node->val = val;
    node->prio = prio;

    return (node);
}

int
queue_append(Queue *queue, QNode *node)
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
    QNode *prev = walk_queue(queue->head, node);

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
queue_remove(Queue *queue, const QNode node)
{
    QNode *removed, *curr, *prev;

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