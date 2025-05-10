#include "../include/cache.h"

Cache* initCache(int capacity, int policy){
    Cache* cache = malloc(sizeof(Cache));
    if(!cache){
        perror("Error alocating memory for the Cache structure.");
        exit(EXIT_FAILURE);
    }

    cache -> docEntry = g_hash_table_new_full(g_int_hash, g_int_equal, NULL, free);
    if(!cache -> docEntry){
        perror("Error alocating memory for the Cache HashTable");
        free(cache);
        exit(EXIT_FAILURE);
    }
    cache -> order = g_queue_new();
    if(!cache -> order){
        perror("Error alocating memory for the Cache Queue");
        free(cache -> docEntry);
        free(cache);
        exit(EXIT_FAILURE);
    }

    cache -> capacity = capacity;
    cache -> current_size = 0;
    cache -> policy = policy; // FIFO or LRU
    cache -> hits = 0;
    cache -> misses = 0;

    return cache;
}

CacheEntry* createEntry(Document* doc){
    CacheEntry* entry = malloc(sizeof(CacheEntry));
    if(!entry){
        perror("Error alocating memory for the Document entry in cache");
        exit(EXIT_FAILURE);
    }
    entry->metaInfo = doc;

    return entry;
}

void insertCache(Cache* cache, int doc_id, Document* doc){
    if(cache && cache -> docEntry && cache -> order){
        // LRU insert
        if(cache -> policy == 1){
            // if already exists, move to tail, so it becomes the recently used doc id
            g_queue_remove(cache -> order, GINT_TO_POINTER(doc_id));
        }
        if(cache -> current_size == cache -> capacity){ // if cache is full
            int oldest_id = GPOINTER_TO_INT(g_queue_pop_head(cache -> order)); // oldest id || least recently used id is removed
            g_hash_table_remove(cache -> docEntry, GINT_TO_POINTER(oldest_id)); // first element added to be removed
            cache->current_size--;
        }
        // insert new doc id to tail
        g_queue_push_tail(cache -> order, GINT_TO_POINTER(doc_id));
        
        CacheEntry* newEntry = createEntry(doc);
        g_hash_table_insert(cache->docEntry, GINT_TO_POINTER(doc_id), newEntry);
        cache->current_size++;
    }
}

CacheEntry* searchCacheEntry(Cache* cache, int doc_id){
    if(cache && cache -> docEntry && cache -> order){
        CacheEntry* entry = g_hash_table_lookup(cache->docEntry, GINT_TO_POINTER(doc_id));
        if(entry){
            cache -> hits++; // entry found
            printf("CACHE HIT: ID %d\n", doc_id);
            if(cache->policy == 1){ // LRU lookup
                g_queue_remove(cache -> order, GINT_TO_POINTER(doc_id));
                g_queue_push_tail(cache -> order, GINT_TO_POINTER(doc_id)); // update order
            }
            return entry;
        }
        printf("CACHE MISS: ID %d\n", doc_id);
        cache -> misses++; // entry not in cache
    }
    return NULL;
}

void removeCacheEntry(Cache* cache, int doc_id){
    if(cache && cache -> docEntry && cache -> order){
        gpointer key = GINT_TO_POINTER(doc_id);
        if(g_hash_table_contains(cache->docEntry, key)){
            g_queue_remove(cache->order, key);
            g_hash_table_remove(cache->docEntry, key);
            cache->current_size--;
        }
        else
            printf("Tried to remove doc_id %d from cache, but it was not found.\n", doc_id); // debug
    }
}

void freeCache(Cache* cache){
    if(cache){
        if(cache -> docEntry){
            g_hash_table_destroy(cache -> docEntry);
        }
        if(cache -> order){
            g_queue_free(cache -> order);
        }
        free(cache);
    }
}

void cacheReport(Cache* cache){
    printf("Cache stats: %d hit(s), %d miss(es), hit rate: %.2f%%\n",
           cache->hits,
           cache->misses,
           cache->hits + cache->misses == 0 ? 0.0 :
           (100.0 * cache->hits / (cache->hits + cache->misses)));
}