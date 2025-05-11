#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "glib.h"

#ifndef DOCUMENT_H
#define DOCUMENT_H

// Defines the max size for the following fields
#define TITLE_SIZE   200
#define AUTHORS_SIZE 200
#define PATH_SIZE    64

// Struct to represent a document
typedef struct{
    char title[TITLE_SIZE];
    char authors[AUTHORS_SIZE];
    char path[PATH_SIZE];
    int year;
    int id; // document's id used as key
}Document;

/**
 * Creates a document
 *  
 * @param title document's title
 * @param authors document's authors, representing single or multiple authors
 * @param path document's path where the document its stored in the user's machine
 * @param id a generated unique ID for each document
 * 
 * @return The document created
 */
Document* createDocument(char* title, char* authors, char* path, int year, int id);

/**
 * Returns a document's ID
 *  
 * @param doc document's structure
 * 
 * @return document's ID
 */
int getDocumentID(Document* doc);

/**
 * Returns a document's title
 * 
 * @param doc document's structure
 * 
 * @return document's title
 */
char* getDocumentTitle(Document* doc);

/**
 * Returns the document's authors
 * 
 * @param doc document's structure
 * 
 * @return document's authors
 */
char* getDocumentAuthors(Document* doc);

/**
 * Returns a document's year
 * 
 * @param doc document's structure
 * 
 * @return document's year
 */
int getDocumentYear(Document* doc);

/**
 * Returns a document's path
 * 
 * @param doc document's structure
 * 
 * @return document's path
 */
char* getDocumentPath(Document* doc);

/**
 * Frees the Document
 * 
 * @param docManager The Document structure to be freed
 */
void freeDocument(Document* doc);

#endif