#include "../include/utils.h"
#include "../include/document/documentManager.h"

int main(int argc, char *argv[]){

    unlink(SERVER);  
    DocumentManager* docManager = initDocumentManager();
    int id_number = 1; // document's ID, later on we would like to update this id_number within our load file

    if(mkfifo(SERVER, 0666)){
        perror("Error creating FIFO.");
        return 1;
    }

    write(1, "Server open...\n", sizeof("Server open...\n"));

    int fd = open(SERVER, O_RDONLY); 
    if(fd < 0){
        perror("Error opening server FIFO.");
        return 1;
    }

    // Keep FIFO open by opening a dummy write descriptor
    int fd_dummy = open(SERVER, O_WRONLY);
    
    Msg msg;
    while(1){  
        int read_bytes = read(fd, &msg, sizeof(msg));
        if(read_bytes < 0){
			perror("Error reading from FIFO.");
            return 0;
        }

        char* commandRequest = commandTypeToString(msg.cmdType);
        printf("Received a resquest from client [PID: %d]: %s %s\n", msg.pid, commandRequest, msg.info);

        if(msg.cmdType == CMD_INDEX){
            char *title = strtok(msg.info, "|");
            char *authors = strtok(NULL, "|");
            char *year_str = strtok(NULL, "|");
            char *path = strtok(NULL, "|");
            int year = atoi(year_str);

            Document* doc = createDocument(title, authors, path, year, id_number);
            indexDocument(docManager, doc);
            snprintf(msg.response, sizeof(msg.response), "Document %d indexed with success.\n", id_number);
            printf("Document %d indexed with success.\n", id_number);
            id_number++; // update so the next document never has the same ID from the previous doc
        }

        if(msg.cmdType == CMD_SEARCH || msg.cmdType == CMD_REMOVE){
            int key = atoi(msg.info);
            if(containsDocumentID(docManager, key)){
                if(msg.cmdType == CMD_SEARCH){
                    Document* doc = findDocument(docManager, key);
                    char* title = getDocumentTitle(doc);
                    char* authors = getDocumentAuthors(doc);
                    int year = getDocumentYear(doc);
                    char* path = getDocumentPath(doc);
                    snprintf(msg.response, sizeof(msg.response), "Title: %s\nAuthors: %s\nYear: %d\nPath: %s\n", title, authors, year, path);
                } else if(msg.cmdType == CMD_SEARCH){
                    removeDocument(docManager, key);
                    snprintf(msg.response, sizeof(msg.response), "Index entry %d deleted with success.\n", key);
                }   
            } else strcpy(msg.response, "Document's ID provided does not exist.\n");
        }

        // answer for client's fifo
        char fifo_name[50];
        sprintf(fifo_name, CLIENT"_%d", msg.pid);

        int fd_client = open(fifo_name, O_WRONLY);
        write(fd_client, &msg, sizeof(msg));

        close(fd_client);
    }

    close(fd);
    close(fd_dummy);
    freeDocumentManager(docManager);
    unlink(SERVER);

    return 0;
}
