#include "../../include/document/document.h"
/* --
    should probably be in utils.h 
    has the purpose to check doc's fields before we create a document structure
    --
int validateDocument(Document* doc){
    if ((strlen(doc->title) > TITLE_SIZE) ||
        (strlen(doc->authors) > AUTHORS_SIZE) ||
        (strlen(doc->path) > PATH_SIZE))
        return 0;
    
    return 1;
}
*/

Document* createDocument(char* title, char* authors, char* path, int year, int id){
    Document* doc = (Document*)malloc(sizeof(Document));
    if(!doc){
        perror("Error alocating memory for a document structure.");
        exit(EXIT_FAILURE);
    }

    strcpy(doc -> title, title);
    strcpy(doc -> authors, authors);
    strcpy(doc -> path, path);

    doc -> year = year;
    doc -> id = id;

    return doc;
}

int getDocumentID(Document* doc){
    return doc -> id;
}

void freeDocument(Document* doc){
    free(doc);
}