#include "../include/document/documentManager.h"
#include "../include/utils.h"
#include "../include/commandRunner.h"

void indexRequest(Msg* msg, DocumentManager* docManager, int* id_number){
    char *title = strtok(msg->info, "|");
    char *authors = strtok(NULL, "|");
    char *year_str = strtok(NULL, "|");
    char *path = strtok(NULL, "|");
    int year = atoi(year_str);

    Document* doc = createDocument(title, authors, path, year, *id_number);
    indexDocument(docManager, doc);
    snprintf(msg->response, sizeof(msg->response), "Document %d indexed with success.\n", *id_number);
    printf("Document %d indexed with success.\n", *id_number);
    (*id_number)++; // update so the next document never has the same ID from the previous doc
}

void searchRequest(Msg* msg, DocumentManager* docManager){
    int key = atoi(msg->info);
    printf("Received Key: %d\n", key);
    if(containsDocumentID(docManager, key)){
        Document* doc = findDocument(docManager, key);
        char* title = getDocumentTitle(doc);
        char* authors = getDocumentAuthors(doc);
        int year = getDocumentYear(doc);
        char* path = getDocumentPath(doc);
        snprintf(msg->response, sizeof(msg->response), "Title: %s\nAuthors: %s\nYear: %d\nPath: %s\n", title, authors, year, path);
    } else strcpy(msg->response, "Document's ID provided does not exist.\n");
}

void removeRequest(Msg* msg, DocumentManager* docManager){
    int key = atoi(msg->info);
    printf("Received Key: %d\n", key);
    if(containsDocumentID(docManager, key)){
        removeDocument(docManager, key);
        printf("Index entry %d deleted with success.\n", key);
        snprintf(msg->response, sizeof(msg->response), "Index entry %d deleted with success.\n", key);
    } else strcpy(msg->response, "Document's ID provided does not exist.\n");
}

void nrlinesRequest(Msg* msg, DocumentManager* docManager, char* doc_folder){
    int key = atoi(strtok(msg->info, "|"));
    char* keyword = strtok(NULL, "|");
    if(containsDocumentID(docManager, key)){
        Document* doc = findDocument(docManager, key);
        char* path = getDocumentPath(doc);
    
        int pipe_fd[2]; // read and write
        pipe(pipe_fd);
        pid_t child = 0;
        if((child = fork()) == 0){
            // run command
            close(pipe_fd[0]);
            dup2(pipe_fd[1], STDOUT_FILENO); // redirect stdout to write-end, because of grep's output
            char command[128]; // assuming a document file path has 64 bytes, the document folder can also be 64 bytes
            snprintf(command, 128, "%s/%s", doc_folder, path); // build path
            char* args[] = {"grep", "-c", keyword, command, NULL};
            execvp("grep", args);
        }
        else{
            // waits for child's answer 
            close(pipe_fd[1]);
            char buffer[64];
            read(pipe_fd[0], buffer, sizeof(buffer)); 
            int nr_lines = atoi(buffer);
            if(nr_lines == 0) snprintf(msg->response, sizeof(msg->response), "Keyword \"%s\" was not found in the document ID %d\n", keyword, key);
            else snprintf(msg->response, sizeof(msg->response), "Keyword \"%s\" found in %d lines of document ID %d.\n", keyword, nr_lines, key);

            close(pipe_fd[0]);
            waitpid(child, NULL, 0);
        }
    }else strcpy(msg->response, "Document's ID provided does not exist.\n");
}

void idlistRequest(Msg* msg, DocumentManager* docManager, char* doc_folder){
    int found = 0; // flag to determine if at least an ID matched keyword
    char* keyword = strdup(msg->info);
    memset(msg->response, 0, sizeof(msg->response));
        
    GHashTableIter iter;
    gpointer key_ptr, value_ptr;

    g_hash_table_iter_init(&iter, docManager->documentTable);

    while(g_hash_table_iter_next(&iter, &key_ptr, &value_ptr)){
        Document* doc = (Document*) value_ptr;
        char* path = getDocumentPath(doc);
        int doc_id = getDocumentID(doc);

        int pipe_fd[2]; // read and write
        pipe(pipe_fd);
        pid_t child = 0;
        if((child = fork()) == 0){
            // run command
            close(pipe_fd[0]);
            dup2(pipe_fd[1], STDOUT_FILENO); // redirect stdout to write-end, because of grep's output
            char command[128]; // assuming a document file path has 64 bytes, the document folder can also be 64 bytes
            snprintf(command, 128, "%s/%s", doc_folder, path); // build path
            char* args[] = {"grep", "-c", keyword, command, NULL};
            execvp("grep", args);
        }
        else{
            // waits for child's answer 
            close(pipe_fd[1]);
            char buffer[64];
            read(pipe_fd[0], buffer, sizeof(buffer)); 
            int nr_lines = atoi(buffer);
            if(nr_lines > 0){
                found = 1;
                char temp[32];
                snprintf(temp, sizeof(temp), "%d ", doc_id);  // format one ID
                strncat(msg->response, temp, sizeof(msg->response) - strlen(msg->response) - 1);
            }
            close(pipe_fd[0]);
            waitpid(child, NULL, 0);
        }

    }
    if(found) strncat(msg->response, "\n", sizeof(msg->response) - strlen(msg->response) - 1); // add a \n
    else snprintf(msg->response, sizeof(msg->response), "No document ID has matched the given keyword: %s\n", keyword);
    free(keyword);
}

void shutdownRequest(Msg* msg, int* isRunning){
    printf("Server is shutting down ...\n");
    snprintf(msg->response, sizeof(msg->response), "Server is shutting down ...\n");
    (*isRunning) = 0;
}