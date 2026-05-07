#ifndef MAP_H
#define MAP_H

#include <pthread.h>

typedef struct CMNode {
    void *id;
    int weight;
    struct CMNode *next;
    struct CMNode *prev;
} CMNode;

/*
 * A concurrent map. ALL reads and mutations of CMap and CMNode should be done
 * with the following methods.
*/
typedef struct {
    int size;
    pthread_mutex_t mtx;
    CMNode *entries;
} CMap;

/* 
 * Remove node from map. 
 *
 * NOT THREAD SAFE.
 * 
 * @returns the current weight of id. 0 if not found or removed.
*/
int remove_node(CMap *map, CMNode *node);

/* 
 * Add an entry to Map or increments the existing entry.
 *
 * @returns the weight of the node if successful, 0 otherwise.
*/
CMNode *add_node(CMap *map, void *id, int weight);

/*
 * Retrieve the weight of id.
 *
 * NOT THREAD SAFE.
 * 
 * @returns 0 if id is not found. weight otherwise.
*/
int get_entry(CMap *map, void *id);

/*
 * Updates the weight of node.
 *
 * @returns the new weight of node (note 0 means it was removed)
*/
int update_node(CMap *map, CMNode *node, int weight);


#endif