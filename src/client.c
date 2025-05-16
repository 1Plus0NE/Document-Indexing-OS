#include "../include/utils.h"

int main(int argc, char * argv[]){

	if(argc < 2){
		printf("Missing argument.\n");
		_exit(1);
	}

	pid_t pid = getpid(); // client's pid
	char client_name[64];
	sprintf(client_name, CLIENT"_%d", pid);

	createFIFO(client_name);

	// Command struct
	Request request;
	request.pid = pid;
	
	// parses the command and builds an error message if an error is handled, otherwise creates the command request
	if(!validateAndBuildMessage(argc, argv, &request, client_name)){
		_exit(1);
	}

	int fd = open(SERVER, O_WRONLY);
	if(fd < 0){
		perror("Error opening server FIFO");
		unlink(client_name);  
		_exit(1);
	}

	if(write(fd, &request, sizeof(Request)) != sizeof(Request)){
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

	int read_bytes = read(fd_client, &request, sizeof(request));
	if(read_bytes <= 0){
		perror("Error reading from server");
		unlink(client_name);
		_exit(1);
	}

	write(1, request.response, strlen(request.response));

	close(fd_client);
	unlink(client_name);

	return 0;
}