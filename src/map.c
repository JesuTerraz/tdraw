#include "map.h"

#include "logger.h"

#include <stdlib.h>

static CMNode *
get_node(CMap *map, void *id)
{
    CMNode *node;
    void *head_id;

    // Bad call.
    if (!map || !id)
        return (NULL);

    // Map empty.
    if (!map->size)
        return (NULL);

    node = map->entries;
    head_id = node->id;
    while (node->id != id)
    {
        node = node->next;

        // We've checked all nodes and couldn't find it.
        if (node->id == head_id)
            return (NULL);
    }

    return (node);
}

int
remove_node(CMap *map, CMNode *node)
{
    if (!map || !node)
        return (-1);

    if (!node->next || !node->prev)
        return (-1);

    if (map->size == 1)
    {
        map->size = 0;
        map->entries = NULL;

        free(node);

        return (0);
    }

    if (node == map->entries)
        map->entries = node->next;

    node->next->prev = node->prev;
    node->prev->next = node->next;

    map->size--;

    free(node);

    return (0);
}

CMNode *
add_node(CMap *map, void *id, int weight)
{
    CMNode *node;

    if (!map || !id)
        return (NULL);

    if (weight <= 0)
        return (NULL);

    node = malloc(sizeof(CMNode));
    if (!node)
        return (NULL);

    node->id = id;
    node->weight = weight;

    pthread_mutex_lock(&map->mtx);

    if (!map->size)
    {
        map->entries = node;
        map->size++;

        node->next = node;
        node->prev = node;

        pthread_mutex_unlock(&map->mtx);
        return (node);
    }

    node->next = map->entries;
    node->prev = map->entries->prev;

    node->prev->next = node;
    map->entries->prev = node;

    map->size++;

    pthread_mutex_unlock(&map->mtx);
    return (node);
}

int
update_node(CMap *map, CMNode *node, int weight)
{
    int rc;

    if (!map || !node)
        return (0);

    pthread_mutex_lock(&map->mtx);

    node->weight += weight;

    if (node->weight <= 0)
    {
        rc = remove_node(map, node);
        pthread_mutex_unlock(&map->mtx);
        return (rc);
    }

    // Return new weight.
    pthread_mutex_unlock(&map->mtx);
    return (node->weight);
}

int
get_entry(CMap *map, void *id)
{
    CMNode *node;
    node = get_node(map, id);

    // Could not find node.
    if (!node)
        return (0);

    return (node->weight);
}