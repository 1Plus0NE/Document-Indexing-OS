#include "../../include/document/documentManager.h"

DocumentManager* initDocumentManager(){
    DocumentManager* docManager = malloc(sizeof(DocumentManager));
    if(!docManager){
        perror("Error alocating memory for DocumentManager structure.");
        exit(EXIT_FAILURE);
    }

    docManager -> documentTable = g_hash_table_new_full(g_int_hash, g_int_equal, free, (GDestroyNotify)freeDocument);

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

bool containsDocumentID(DocumentManager* docManager, int key){
    if(docManager && docManager -> documentTable){
        return g_hash_table_contains(docManager -> documentTable, &key);
    }
    return false;
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

void saveDocuments(DocumentManager* docManager, char* file_docs){
    int fd = open(file_docs, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fd < 0){
        perror("Error opening file");
        return;
    }

    GHashTableIter iter;
    gpointer key_ptr, value_ptr;
    ssize_t bytes_written;

    g_hash_table_iter_init(&iter, docManager->documentTable);

    while(g_hash_table_iter_next(&iter, &key_ptr, &value_ptr)){
        Document* doc = (Document*) value_ptr;
        bytes_written = write(fd, doc, sizeof(Document));
        if(bytes_written != sizeof(Document)){
            perror("Error writing to file");
            return;
        }
    } 
    close(fd);
}

int loadDocuments(DocumentManager* docManager, char* file_docs){
    int fd = open(file_docs, O_RDONLY);
    if(fd < 0){
        perror("File does not exist. Creating a new one...");
        fd = open(file_docs, O_CREAT | O_RDWR, 0644);
        if(fd < 0){
            perror("Error creating file");
            exit(EXIT_FAILURE);
        }
        close(fd);
        return 1;
    }

    Document doc;
    int id_count = 1, document_id;

    while(read(fd, &doc, sizeof(Document)) == sizeof(Document)){
        Document* loadedDocument = malloc(sizeof(Document));
        if(!loadedDocument){
            perror("Error allocating memory for the document");
            close(fd);
            return 0;
        }
        memcpy(loadedDocument, &doc, sizeof(Document));
        indexDocument(docManager, loadedDocument);
        document_id = getDocumentID(loadedDocument);
        if(document_id >= id_count)
            id_count = document_id + 1;
    }
    close(fd);
    return id_count; 
} 

void freeDocumentManager(DocumentManager* docManager){
    if(docManager){
        if(docManager -> documentTable){
            g_hash_table_destroy(docManager -> documentTable);
        }
        free(docManager);
    }
}