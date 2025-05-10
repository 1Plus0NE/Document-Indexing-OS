#include "document.h"
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#ifndef CACHE_H
#define CACHE_H

/**
 * Structure to represent the Cache
 */
typedef struct{
    GHashTable* docEntry;   // key: doc_id, value: CacheEntry*
    GQueue* order;          // FIFO or LRU order tracking
    int capacity;       
    int current_size;   
    int policy;             // 0 -> FIFO | 1 -> LRU
    int hits;
    int misses;
}Cache;

/**
 * Structure that stores a Document in the Cache
 */
typedef struct{
    Document* metaInfo;
}CacheEntry;

/**
 * Initializes the Cache, the docEntry GHashTable, the order GQueue
 * Sets the capacity and policy with the given arguments
 * 
 * @return The initialazed Cache
 */
Cache* initCache(int capacity, int policy);

/**
 * Initializes the CacheEntry that includes the Document
 * 
 * @return The initialized CacheEntry
 */
CacheEntry* createEntry(Document* doc);

/**
 * Inserts a Document to the cache regarding the FIFO and LRU policies
 * In case of LRU policy, the function begins to remove the doc_id from the queue,
 * which will be later re-added to the queue's tail in order to become the most recently used element
 * 
 * Regardless of the policy, if the cache is full, the program has to: 
 * - gather the oldest/least recently used id and remove it from the queue's head;
 * - remove the entry from the hash table (docEntry);
 * - decrement cache's size
 * 
 * In case the cache isnt full, the program inserts the id to the queue's tail,
 * a newEntry is created and inserted in the hash table and the cache's size is incremented
 */
void insertCache(Cache* cache, int doc_id, Document* doc);

/**
 * Retrieves a Cache Entry given a document id
 * In case the document id matches an existing entry in the cache: 
 * - for LRU policy, its order is updated so it becomes the most recently used element;
 * - hits are incremented
 * 
 * Otherwise, misses are incremented
 * 
 * @return The entry found in the cache
 */
CacheEntry* searchCacheEntry(Cache* cache, int doc_id);

/**
 * Removes a entry from the cache that matches an existing entry and updates cache's size
 */
void removeCacheEntry(Cache* cache, int doc_id);

/**
 * Frees all memory allocated on the Cache structure
 */
void freeCache(Cache* cache);

/**
 * Prints a resume regarding hits, misses and hit rate in the cache
 */
void cacheReport(Cache* cache);

#endif