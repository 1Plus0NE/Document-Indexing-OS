#include "../include/utils.h"

int main (int argc, char * argv[]){

	if(argc < 2){
		printf("Missing argument.\n");
		_exit(1);
	}

	pid_t pid = getpid(); // client's pid
	char client_name[64];
	sprintf(client_name, CLIENT"_%d", pid);

	if(mkfifo(client_name, 0644) < 0){
		perror("Error creating fifo");
		_exit(1);
	}

	// aux struct
	Msg msg;
	msg.pid = pid;
	strncpy(msg.info, argv[1], sizeof(msg.info) - 1); // simulation of a command to be sent/executed in the server
    msg.info[sizeof(msg.info) - 1] = '\0'; 

	int fd = open(SERVER, O_WRONLY);
	if(fd < 0){
		perror("Error opening server FIFO");
		unlink(client_name);  
		_exit(1);
	}

	if(write(fd, &msg, sizeof(Msg)) != sizeof(Msg)){
		perror("Error writing to server");
		unlink(client_name); 
		_exit(1);
	}

	close(fd);

	int fd_client = open(client_name, O_RDONLY);
	if(fd_client < 0){
		perror("Error opening client FIFO");
		unlink(client_name);
		_exit(1);
	}

	int read_bytes = read(fd_client, &msg, sizeof(msg));
	if(read_bytes <= 0){
		perror("Error reading from server");
		unlink(client_name);
		_exit(1);
	}

	printf("Server received my message: %s\n", msg.info);

	close(fd_client);
	unlink(client_name);

	return 0;
}

