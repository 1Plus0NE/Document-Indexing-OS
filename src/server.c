#include "../include/utils.h"
#include "../include/document/documentManager.h"
#include "../include/commandRunner.h"
#include "../include/cache.h"

int main(int argc, char *argv[]){
  
    if(argc < 3){
        char* usage = "Missing argument!\nUsage: ./dserver <document-folder> <cache_size>\n";
        write(1, usage, strlen(usage));
        return 0;
    }

    char* doc_folder = argv[1];
    if(!verifyDirectory(doc_folder)){
        perror("Invalid document folder");
        return 0;
    }

    unlink(SERVER);
    DocumentManager* docManager = initDocumentManager();
    int id_number = loadDocuments(docManager, DOCUMENTS);; // document's ID
    int isRunning = 1;
    int capacity = atoi(argv[2]);
    Cache* cache = initCache(capacity, 0); // 0 -> FIFO

    createFIFO(SERVER);
    write(1, "Server open ...\n", sizeof("Server open ...\n"));
    int server_fifo = openFIFO(SERVER, O_RDONLY);

    // Keep FIFO open by opening a dummy write descriptor
    int dummy_fifo = openFIFO(SERVER, O_WRONLY);

    Msg msg;
    while(isRunning){
        int read_bytes = read(server_fifo, &msg, sizeof(msg));
        if(read_bytes < 0){
            perror("Error reading from FIFO.");
            break;
        }

        // feedback from client's request
        char* commandRequest = commandTypeToString(msg.cmdType);
        printf("Received a request from client [PID: %d]: %s %s\n", msg.pid, commandRequest, msg.info);

        switch(msg.cmdType){
            case CMD_INDEX:
            case CMD_REMOVE:
            case CMD_SHUTDOWN:
            { // handle basic commands in parent
                if(msg.cmdType == CMD_INDEX)
                    indexRequest(&msg, docManager, &id_number);
                else if(msg.cmdType == CMD_REMOVE)
                    removeRequest(&msg, docManager);
                else if(msg.cmdType == CMD_SHUTDOWN)
                    shutdownRequest(&msg, &isRunning);

                // answer to client's fifo
                char fifo_name[50];
                sprintf(fifo_name, CLIENT"_%d", msg.pid);
                int fd_client = openFIFO(fifo_name, O_WRONLY);
                write(fd_client, &msg, sizeof(msg));
                close(fd_client);
            } break;

            case CMD_SEARCH:
            case CMD_NRLINES:
            case CMD_IDLIST:
            { // Handle search commands in a forked child, so we prevent a client to be stuck
                pid_t pid = fork();
                if(pid == 0){
                    // Child process
                    if(msg.cmdType == CMD_SEARCH)
                        searchRequest(&msg, docManager);
                    else if(msg.cmdType == CMD_NRLINES)
                        nrlinesRequest(&msg, docManager, doc_folder);
                    else if(msg.cmdType == CMD_IDLIST){
                        char* keyword = strtok(msg.info, "|");
                        char* nr_processes_str = strtok(NULL, "|");
                        int nr_processes = atoi(nr_processes_str);
                        strcpy(msg.info, keyword);
                    
                        if(nr_processes < 1) // nr_processes cannot be negative
                            snprintf(msg.response, sizeof(msg.response), "Invalid number of processes: %d\n", nr_processes);                           
                        else if(nr_processes == 1) // single process 
                            idlistRequest(&msg, docManager, doc_folder);
                        else // N processes 
                            idlistProcessesRequest(&msg, docManager, doc_folder, nr_processes);
                    }
                    // answer to client's fifo
                    char fifo_name[50];
                    sprintf(fifo_name, CLIENT"_%d", msg.pid);
                    int fd_client = openFIFO(fifo_name, O_WRONLY);
                    write(fd_client, &msg, sizeof(msg));
                    close(fd_client);

                    exit(0); // Child done
                }
            } break;

            default:
                // Never happens supposedly
                break;
        }
    }

    close(server_fifo);
    close(dummy_fifo);
    saveDocuments(docManager, DOCUMENTS);
    freeDocumentManager(docManager);
    freeCache(cache);
    unlink(SERVER);

    return 0;
}