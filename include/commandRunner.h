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

/**
 * Struct responsible to keep track of a process
 */
typedef struct{
    pid_t pid;
    int pipe_fd;
    int doc_id;
}ProcInfo;

/**
 * Indexes a document into the DocumentManager structure and updates the Document ID counter
 * 
 * @param Msg pointer to the Msg structure containing the indexing request (msg.cmdTye) and where the response will be stored (msg.response)
 * @param DocManager pointer to the DocumentManager structure that manages all documents
 * @param id_number pointer to the current document ID counter, which is updated whenever a document is indexed
 */
void indexRequest(Msg* msg, DocumentManager* docManager, int* id_number);

/**
 * Searches for a document in the DocumentManager structure and send its info through Msg structure
 * 
 * @param Msg pointer to the Msg structure containing the searching request (msg.cmdTye) and where the response will be stored (msg.response)
 * @param DocManager pointer to the DocumentManager structure that manages all documents
 */
void searchRequest(Msg* msg, DocumentManager* docManager);

/**
 * Removes a document from the DocumentManager structure
 * 
 * @param Msg pointer to the Msg structure containing the remove request (msg.cmdTye) and where the response will be stored (msg.response)
 * @param DocManager pointer to the DocumentManager structure that manages all documents
 */
void removeRequest(Msg* msg, DocumentManager* docManager);

/**
 * Counts the document's number of lines, in the DocumentManager structure, that matches a given keyword
 * 
 * @param Msg pointer to the Msg structure containing the count lines request (msg.cmdTye) and where the response will be stored (msg.response)
 * @param DocManager pointer to the DocumentManager structure that manages all documents
 * @param doc_folder path where client's documents are locally stored
 */
void nrlinesRequest(Msg* msg, DocumentManager* docManager, char* doc_folder);

/**
 * Counts the document's IDs, requested in the DocumentManager structure, where their number of lines is at least greater than 0
 * 
 * @param Msg pointer to the Msg structure containing the id list request (msg.cmdTye) and where the response will be stored (msg.response)
 * @param DocManager pointer to the DocumentManager structure that manages all documents
 * @param doc_folder path where client's documents are locally stored
 */
void idlistRequest(Msg* msg, DocumentManager* docManager, char* doc_folder);

/**
 * Processes an ID list request concurrently by scanning all documents in the DocumentManager.
 * Using up to "nr_processes" to search for a given keyword across all indexed documents.
 * 
 * @param Msg pointer to the Msg structure containing the id list request (msg.cmdTye) and where the response will be stored (msg.response)
 * @param DocManager pointer to the DocumentManager structure that manages all documents
 * @param doc_folder path where client's documents are locally stored
 * @param nr_processes the number of processes to run simultaneously
 */ 
void idlistProcessesRequest(Msg* msg, DocumentManager* docManager, char* doc_folder, int nr_processes);

/**
 * Request to shutdown the server and updates isRunning variable
 * 
 * @param Msg pointer to the Msg structure containing the shutdown request (msg.cmdTye) and where the response will be stored (msg.response)
 * @param isRunning pointer that represents if the server is running or not
 */
void shutdownRequest(Msg* msg, int* isRunning);

#endif 