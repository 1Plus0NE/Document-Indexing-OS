#include "../include/utils.h"

int main(int argc, char *argv[]){

    unlink(SERVER);  

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

        printf("Received a message from client [PID: %d]: %s\n", msg.pid, msg.info);

        // answer for client's fifo
        char fifo_name[50];
        sprintf(fifo_name, CLIENT"_%d", msg.pid);

        int fd_client = open(fifo_name, O_WRONLY);
        write(fd_client, &msg, sizeof(msg));

        close(fd_client);
    }

    close(fd);
    close(fd_dummy);
    unlink(SERVER);

    return 0;
}
