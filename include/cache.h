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
 * @param capacity The capacity of the cache
 * @param policy The policy of the cache, FIFO or LRU
 * 
 * @return The initialazed Cache
 */
Cache* initCache(int capacity, int policy);

/**
 * Initializes the CacheEntry that includes the Document
 * 
 * @param doc The document to be inserted in the cache entry
 * 
 * @return The initialized CacheEntry
 */
CacheEntry* createEntry(Document* doc);

/**
 * Inserts a Document to the cache regarding the FIFO and LRU policies.
 * Misses are incremented
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
 * 
 * @param cache The cache structure
 * @param doc_id The document id
 * @param doc The document
 */
void insertCache(Cache* cache, int doc_id, Document* doc);

/**
 * Retrieves a Cache Entry given a document id
 * In case the document id matches an existing entry in the cache: 
 * - for LRU policy, its order is updated so it becomes the most recently used element;
 * - hits are incremented
 * 
 * @param cache The cache structure
 * @param doc_id The document id
 * @param readonly The flag for the child process
 * 
 * @return The entry found in the cache
 */
CacheEntry* searchCacheEntry(Cache* cache, int doc_id, int readonly);

/**
 * Removes a entry from the cache that matches an existing entry and updates cache's size
 * 
 * @param cache The cache structure
 * @param doc_id The document id
 */
void removeCacheEntry(Cache* cache, int doc_id);

/**
 * Frees all memory allocated on the Cache structure
 * 
 * @param cache The cache structure
 */
void freeCache(Cache* cache);

/**
 * Prints a resume regarding hits, misses and hit rate in the cache
 * 
 * @param cache The cache structure
 */
void cacheReport(Cache* cache);

#endif