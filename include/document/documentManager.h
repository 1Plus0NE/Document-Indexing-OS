#ifndef DOCUMENTMANAGER_H
#define DOCUMENTMANAGER_H

#include "document.h"
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

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
 * Removes a document from the DocumentManager's GHashTable
 * 
 * @param docManager The structure to remove the document
 * @param doc The document to be removed
 */
void removeDocument(DocumentManager* docManager, Document* doc);

/**
 * Finds a document in the DocumentManager by its key
 * 
 * @param docManager The structure to find the document
 * @param key The key of the document
 * 
 * @return The document found
 */
Document* findDocument(DocumentManager* docManager, int key);

/**
 * Frees the DocumentManager
 * 
 * @param docManager The DocumentManager structure to be freed
 * 
 */
void freeDocumentManager(DocumentManager* docManager)

#endif

