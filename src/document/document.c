#include "../../include/document/document.h"

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

char* getDocumentTitle(Document* doc){
    return doc -> title;
}

char* getDocumentAuthors(Document* doc){
    return doc -> authors;
}

int getDocumentYear(Document* doc){
    return doc -> year;
}

char* getDocumentPath(Document* doc){
    return doc -> path;
}

void freeDocument(Document* doc){
    g_free(doc);
}