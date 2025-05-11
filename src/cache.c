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
        printf("CACHE MISS: ID %d\n", doc_id);
        cache -> misses++;

        int* key_ptr = malloc(sizeof(int));
        if(!key_ptr){
            perror("Failed to allocate memory for cache key");
            return;
        }
        *key_ptr = doc_id;

        // LRU insert
        if(cache -> policy == 1){
            // if already exists, move to tail, so it becomes the recently used doc id
            for(GList* l = cache->order->head; l != NULL; l = l->next){ // loop through the list that represents the queue
                if(*((int*)l->data) == doc_id){
                    g_queue_remove(cache->order, l->data);
                    free(l->data); // free the old key
                    break;
                }
            }
        }
        if(cache -> current_size == cache -> capacity){ // if cache is full
            int* oldest_key = g_queue_pop_head(cache->order); // oldest id || least recently used id is removed
            g_hash_table_remove(cache->docEntry, oldest_key);
            free(oldest_key);
            cache->current_size--;
        }
        // insert new doc id to tail
        g_queue_push_tail(cache->order, key_ptr);
        
        CacheEntry* newEntry = createEntry(doc);
        g_hash_table_insert(cache->docEntry, key_ptr, newEntry);
        cache->current_size++;
    }
}

CacheEntry* searchCacheEntry(Cache* cache, int doc_id, int readonly){
    if(!cache || !cache->docEntry || !cache->order) return NULL;

    // Search for the key in the queue
    int* found_key = NULL;
    for(GList* l = cache->order->head; l != NULL; l = l->next){ 
        if(*((int*)l->data) == doc_id){
            found_key = l->data;
            break;
        }
    }

    if(found_key){
        CacheEntry* entry = g_hash_table_lookup(cache->docEntry, found_key);
        if(entry && !readonly){ // Parent stuff
            printf("CACHE HIT: ID %d\n", doc_id);
            cache->hits++;
            if(cache->policy == 1){ // LRU lookup
                g_queue_remove(cache->order, found_key);
                int* new_key = malloc(sizeof(int));
                *new_key = doc_id;
                g_queue_push_tail(cache->order, new_key);
                g_hash_table_replace(cache->docEntry, new_key, entry);
                free(found_key); // free old key
            }
        }
        return entry; // child process just returns entry
    }

    return NULL;
}

void removeCacheEntry(Cache* cache, int doc_id){
    if(!cache || !cache->docEntry || !cache->order) return;

    for(GList* l = cache->order->head; l != NULL; l = l->next){ // loop through the list that represents the key
        if(*((int*)l->data) == doc_id){
            int* key = l->data;
            g_queue_remove(cache->order, key);
            g_hash_table_remove(cache->docEntry, key);
            free(key);
            cache->current_size--;
            return;
        }
    }
    printf("Tried to remove doc_id %d from cache, but it was not found.\n", doc_id); // debug
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