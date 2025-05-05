#include "../include/utils.h"
#include "../include/document/documentManager.h"
#include "../include/commandRunner.h"

int main(int argc, char *argv[]){

    unlink(SERVER);  
    DocumentManager* docManager = initDocumentManager();
    char* doc_folder = argv[1];
    int id_number = loadDocuments(docManager, DOCUMENTS);; // document's ID
    int isRunning = 1;

    if(argc < 2){
        char* usage = "Missing argument!\nUsage: ./server <document-folder>\n";
        write(1, usage, strlen(usage));
        return 0;
    }

    if(!verifyDirectory(doc_folder)){
        perror("Invalid document folder");
        return 0;
    }

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
            return 0;
        }

        // feedback from client's request
        char* commandRequest = commandTypeToString(msg.cmdType);
        printf("Received a resquest from client [PID: %d]: %s %s\n", msg.pid, commandRequest, msg.info);

        switch(msg.cmdType){
            case CMD_INDEX:
                indexRequest(&msg, docManager, &id_number);
                break;
            case CMD_SEARCH:
                searchRequest(&msg, docManager);
                break;
            case CMD_REMOVE:
                removeRequest(&msg, docManager);
                break;
            case CMD_NRLINES:
                nrlinesRequest(&msg, docManager, doc_folder);
                break;
            case CMD_IDLIST:
                idlistRequest(&msg, docManager, doc_folder);
                break;
            case CMD_SHUTDOWN:
                shutdownRequest(&msg, &isRunning);
                break;
            default: // never happens supposedly
                break; 
        }

        // answer for client's fifo
        char fifo_name[50];
        sprintf(fifo_name, CLIENT"_%d", msg.pid);

        int fd_client = openFIFO(fifo_name, O_WRONLY);
        write(fd_client, &msg, sizeof(msg));

        close(fd_client);
    }

    close(server_fifo);
    close(dummy_fifo);
    saveDocuments(docManager, DOCUMENTS);
    freeDocumentManager(docManager);
    unlink(SERVER);

    return 0;
}