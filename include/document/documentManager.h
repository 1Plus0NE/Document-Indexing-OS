#include "document.h"
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <glib.h>

#ifndef DOCUMENTMANAGER_H
#define DOCUMENTMANAGER_H

// Struct to store all documents indexed in a GHashTable
typedef struct{
    GHashTable* documentTable;
}DocumentManager;

/**
 * Initializes a GHashTable, encapsulated in a structure, responsible to store all documents to be indexed
 * 
 * @return The structure created
 */
DocumentManager* initDocumentManager();

/**
 * Indexes a document in the DocumentManager's GHashTable
 * 
 * @param docManager The structure to index the document
 * @param doc The document to be indexed 
 */
void indexDocument(DocumentManager* docManager, Document* doc);

/**
 * Checks if a document with the given key exists
 * 
 * @param docManager The structure to check if key corresponds to a document
 * @param key The document key to check for existence
 * 
 * @return TRUE if a document with the specified key exists, FALSE otherwise
 */
bool containsDocumentID(DocumentManager* docManager, int key);

/**
 * Removes a document from the DocumentManager's GHashTable
 * 
 * @param docManager The structure to remove the document
 * @param key The key that identifies its document to be removed
 */
void removeDocument(DocumentManager* docManager, int key);

/**
 * Finds a document in the DocumentManager by its key
 * 
 * @param docManager The structure to find the document
 * @param key The key of the document
 * 
 * @return The document found
 */
Document* findDocument(DocumentManager* docManager, int key);

void saveDocuments(DocumentManager* docManager, char* file_docs);

int loadDocuments(DocumentManager* docManager, char* file_docs);

/**
 * Frees the DocumentManager
 * 
 * @param docManager The DocumentManager structure to be freed
 * 
 */
void freeDocumentManager(DocumentManager* docManager);

#endif

