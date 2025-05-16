#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#ifndef COMMANDRUNNER_H
#define COMMANDRUNNER_H

#include "cache.h"

/**
 * Struct responsible to keep track of a process
 */
typedef struct{
    pid_t pid;
    int pipe_fd;
    int doc_id;
}ProcInfo;

/**
 * Struct responsible to give Cache feedback
 */
typedef struct{
    int doc_id;
    int was_hit; // 1 = hit, 0 = miss
}CacheResult;

/**
 * Indexes a document into the DocumentManager structure and updates the Document ID counter
 * 
 * @param Request pointer to the Request structure containing the indexing request (request.cmdTye) and where the response will be stored (request.response)
 * @param DocManager pointer to the DocumentManager structure that manages all documents
 * @param id_number pointer to the current document ID counter, which is updated whenever a document is indexed
 */
void indexRequest(Request* request, DocumentManager* docManager, int* id_number);

/**
 * Searches for a document both in Cache and in the DocumentManager structure and send its info through Request structure
 * If a document is in the Cache it is returned via Cache, otherwise via DocumentManager
 * 
 * @param Request pointer to the Request structure containing the searching request (request.cmdTye) and where the response will be stored (request.response)
 * @param DocManager pointer to the DocumentManager structure that manages all documents
 * @param Cache pointer to the Cache structure that stores documents depending on its size
 * @param notify_fd pipe fd to write the result
 */
void searchRequest(Request* request, DocumentManager* docManager, Cache* cache, int notify_fd);

/**
 * Removes a document from the DocumentManager structure
 * 
 * @param Request pointer to the Request structure containing the remove request (request.cmdTye) and where the response will be stored (request.response)
 * @param DocManager pointer to the DocumentManager structure that manages all documents
 * @param Cache pointer to the Cache structure that stores documents depending on its size
 */
void removeRequest(Request* request, DocumentManager* docManager, Cache* cache);

/**
 * Counts the document's number of lines, in the DocumentManager structure, that matches a given keyword
 * 
 * @param Request pointer to the Request structure containing the count lines request (request.cmdTye) and where the response will be stored (request.response)
 * @param DocManager pointer to the DocumentManager structure that manages all documents
 * @param doc_folder path where client's documents are locally stored
 */
void nrlinesRequest(Request* request, DocumentManager* docManager, char* doc_folder);

/**
 * Counts the document's IDs, requested in the DocumentManager structure, where their number of lines is at least greater than 0
 * 
 * @param Request pointer to the Request structure containing the id list request (request.cmdTye) and where the response will be stored (request.response)
 * @param DocManager pointer to the DocumentManager structure that manages all documents
 * @param doc_folder path where client's documents are locally stored
 */
void idlistRequest(Request* request, DocumentManager* docManager, char* doc_folder);

/**
 * Processes an ID list request concurrently by scanning all documents in the DocumentManager.
 * Using up to "nr_processes" to search for a given keyword across all indexed documents.
 * 
 * @param Request pointer to the Request structure containing the id list request (request.cmdTye) and where the response will be stored (request.response)
 * @param DocManager pointer to the DocumentManager structure that manages all documents
 * @param doc_folder path where client's documents are locally stored
 * @param nr_processes the number of processes to run simultaneously
 */ 
void idlistProcessesRequest(Request* request, DocumentManager* docManager, char* doc_folder, int nr_processes);

/**
 * Request to shutdown the server and updates isRunning variable
 * 
 * @param Request pointer to the Request structure containing the shutdown request (request.cmdTye) and where the response will be stored (request.response)
 * @param isRunning pointer that represents if the server is running or not
 */
void shutdownRequest(Request* request, int* isRunning);

#endif 