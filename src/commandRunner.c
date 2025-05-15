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

void searchRequest(Msg* msg, DocumentManager* docManager, Cache* cache, int notify_fd){
    int key = atoi(msg->info);
    gettimeofday(&msg->start_time, NULL);
    CacheEntry* entry = searchCacheEntry(cache, key, 1); // child is readonly

    // Arguments to store for cache feedback
    CacheResult result = {
        .doc_id = key,
        .was_hit = (entry != NULL) // hit if entry exists, miss otherwise
    };

    if(entry){
        struct timeval end_time, duration;
        gettimeofday(&end_time, NULL);  
        timersub(&end_time, &msg->start_time, &duration); 
        Document* doc = entry->metaInfo;
        snprintf(msg->response, sizeof(msg->response), "Title: %s\nAuthors: %s\nYear: %d\nPath: %s\nDocument retrieved from cache within %ld.%06ld seconds\n",
                 getDocumentTitle(doc),
                 getDocumentAuthors(doc),
                 getDocumentYear(doc),
                 getDocumentPath(doc),
                 duration.tv_sec,
                 duration.tv_usec);
    }
    else if(containsDocumentID(docManager, key)){
        Document* doc = findDocument(docManager, key);
        struct timeval end_time, duration;
        gettimeofday(&end_time, NULL);  
        timersub(&end_time, &msg->start_time, &duration); 
        snprintf(msg->response, sizeof(msg->response), "Title: %s\nAuthors: %s\nYear: %d\nPath: %s\nDocument retrieved from Document Manager within  %ld.%06ld seconds\n",
                 getDocumentTitle(doc),
                 getDocumentAuthors(doc),
                 getDocumentYear(doc),
                 getDocumentPath(doc),
                 duration.tv_sec,
                 duration.tv_usec);
    } else strcpy(msg->response, "Document's ID provided does not exist.\n");
    
    // Send cache feedback to parent
    write(notify_fd, &result, sizeof(result));
    close(notify_fd);
}

void removeRequest(Msg* msg, DocumentManager* docManager, Cache* cache){
    int key = atoi(msg->info);
    printf("Received Key: %d\n", key);
    if(containsDocumentID(docManager, key)){
        removeCacheEntry(cache, key); // remove the entry from the cache in case exists
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
                if(!found)
                    strncat(msg->response, "[", sizeof(msg->response) - strlen(msg->response) - 1); // begin to add a "[""
                 else 
                    strncat(msg->response, ", ", sizeof(msg->response) - strlen(msg->response) - 1); // enumerate each id found
                
                found = 1;

                char temp[32];
                snprintf(temp, sizeof(temp), "%d", doc_id);
                strncat(msg->response, temp, sizeof(msg->response) - strlen(msg->response) - 1);
            }
            close(pipe_fd[0]);
            waitpid(child, NULL, 0);
        }

    }
    if(found) strncat(msg->response, "]\n", sizeof(msg->response) - strlen(msg->response) - 1); // add a \n
    else snprintf(msg->response, sizeof(msg->response), "No document ID has matched the given keyword: %s\n", keyword);
    free(keyword);
}

void idlistProcessesRequest(Msg* msg, DocumentManager* docManager, char* doc_folder, int nr_processes){
    int active_processes = 0;
    int found = 0;
    char* keyword = strdup(msg->info);
    memset(msg->response, 0, sizeof(msg->response));

    GHashTableIter iter;
    gpointer key_ptr, value_ptr;

    g_hash_table_iter_init(&iter, docManager->documentTable);

    // Struct to keep track of each process
    ProcInfo* proc_list = NULL;
    int proc_capacity = 0;
    int proc_index = 0;

    while(g_hash_table_iter_next(&iter, &key_ptr, &value_ptr)){
        Document* doc = (Document*) value_ptr;
        char* path = getDocumentPath(doc);
        int doc_id = getDocumentID(doc);

        int pipe_fd[2]; // read and write
        pipe(pipe_fd);
        // in case pipe fails
        if(pipe(pipe_fd) == -1){
            perror("pipe failed");
            free(keyword);
            free(proc_list);
            exit(EXIT_FAILURE);
        }

        pid_t child = fork();
        // in case fork child fails
        if(child == -1){
            perror("fork failed");
            close(pipe_fd[0]); // close read-end
            close(pipe_fd[1]); // close write-end
            free(keyword);
            free(proc_list);
            exit(EXIT_FAILURE);
        }
        // Child process
        if(child == 0){
            close(pipe_fd[0]); // close read end
            dup2(pipe_fd[1], STDOUT_FILENO); // redirect stdout to write-end, because of grep's output
            close(pipe_fd[1]); // close write-end after dup2

            char command[128]; // assuming a document file path has 64 bytes, the document folder can also be 64 bytes
            snprintf(command, sizeof(command), "%s/%s", doc_folder, path); // build path
            char* args[] = {"grep", "-c", keyword, command, NULL};
            execvp("grep", args);
            _exit(1); // exec failed
        // Parent process
        }else{
            close(pipe_fd[1]); // Close write end
            if(proc_index >= proc_capacity){
                proc_capacity = proc_capacity == 0 ? 16 : proc_capacity * 2; // if capacity is 0 then initialize as 16, otherwise double it
                proc_list = realloc(proc_list, proc_capacity * sizeof(ProcInfo));
                if(!proc_list){
                    perror("realloc failed");
                    exit(EXIT_FAILURE);
                }
            }
            // store all info about the process
            proc_list[proc_index].pid = child; // its pid
            proc_list[proc_index].pipe_fd = pipe_fd[0]; // result from grep
            proc_list[proc_index].doc_id = doc_id; // doc id will be used in case the result from grep is greater than 0
            proc_index++;
            active_processes++;

            // Limit concurrent processes
            if(active_processes >= nr_processes){
                int status;
                wait(&status);
                active_processes--;
            }
        }
    }

    // Wait for remaining children
    while(active_processes > 0){
        int status;
        wait(&status);
        active_processes--;
    }

    // Read from all pipes
    for(int i = 0; i < proc_index; i++){
        char buffer[64];
        ssize_t bytes_read = read(proc_list[i].pipe_fd, buffer, sizeof(buffer));
        close(proc_list[i].pipe_fd);
        if(bytes_read > 0){
            int nr_lines = atoi(buffer);
            if(nr_lines > 0){
                if(!found)
                    strncat(msg->response, "[", sizeof(msg->response) - strlen(msg->response) - 1); // begin to add a "[""
                 else 
                    strncat(msg->response, ", ", sizeof(msg->response) - strlen(msg->response) - 1); // enumerate each id found
                
                found = 1;

                char temp[32];
                snprintf(temp, sizeof(temp), "%d", proc_list[i].doc_id);
                strncat(msg->response, temp, sizeof(msg->response) - strlen(msg->response) - 1);
            }
        }
    }

    if(found) strncat(msg->response, "]\n", sizeof(msg->response) - strlen(msg->response) - 1); // at least an id found
    else snprintf(msg->response, sizeof(msg->response), "No document ID has matched the given keyword: %s\n", keyword); // no id found
    free(keyword);
    free(proc_list);
}

void shutdownRequest(Msg* msg, int* isRunning){
    printf("Server is shutting down ...\n");
    snprintf(msg->response, sizeof(msg->response), "Server is shutting down ...\n");
    (*isRunning) = 0;
}