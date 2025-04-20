#include "../../include/document/documentManager.h"

DocumentManager* initDocumentManager(){
    DocumentManager* docManager = malloc(sizeof(DocumentManager));
    if(!docManager){
        perror("Error alocating memory for DocumentManager structure.");
        exit(EXIT_FAILURE);
    }

    docManager -> documentTable = g_hash_table_new_full(g_int_hash, g_int_equal, free, freeDocument);

    return docManager;
}

void indexDocument(DocumentManager* docManager, Document* doc){
    if(docManager && docManager -> documentTable){
        int* key = malloc(sizeof(int));
        if(!key){
            perror("Error alocating memory for document's key.");
            exit(EXIT_FAILURE);
        }
        *key = getDocumentID(doc);
        g_hash_table_insert(docManager -> documentTable, key, doc);
    }
}

void removeDocument(DocumentManager* docManager, int key){
    if(docManager && docManager -> documentTable){
        g_hash_table_remove(docManager -> documentTable, &key);
    }
}

Document* findDocument(DocumentManager* docManager, int key){
    if(docManager && docManager -> documentTable){
        return (Document*) g_hash_table_lookup(docManager -> documentTable, &key);
    }
    return NULL;
}

void freeDocumentManager(DocumentManager* docManager){
    if(docManager){
        if(docManager -> documentTable){
            g_hash_table_destroy(docManager -> documentTable);
        }
        free(docManager);
    }
}