#include "../include/utils.h"

int verifyDirectory(char* dir_path){
    // Opens a directory to verify if it exists
    int fd = open(dir_path, O_RDONLY | __O_DIRECTORY);
    if(fd < 0){
        return 0;
    }
    close(fd);
    return 1;
}

int createFIFO(char* name){
    // Creates a FIFO with the given name
    int result = mkfifo(name, 0644);
    if(result < 0){
      perror("Error creating FIFO");
      return 0;
    }
    return 1;
}

int openFIFO(char* name, int mode){
    // Opens a FIFO with the given name
    int result = open(name, mode);
    if(result < 0){
      perror("Error opening FIFO");
      return 0;
    }
    return result;
}

int validateFields(char* title, char* authors, char* path){
    if((strlen(title) > TITLE_SIZE) ||
        (strlen(authors) > AUTHORS_SIZE) ||
        (strlen(path) > PATH_SIZE))
        return 0;
    
    return 1;
}

CommandType parseCommand(char* command){
    if(strcmp(command, "-a") == 0) return CMD_INDEX;
    if(strcmp(command, "-d") == 0) return CMD_REMOVE;
    if(strcmp(command, "-c") == 0) return CMD_SEARCH;
    if(strcmp(command, "-l") == 0) return CMD_NRLINES;
    return CMD_INVALID;
}

char* commandTypeToString(CommandType cmd){
    switch(cmd){
        case CMD_INDEX:
            return "-a";
        case CMD_REMOVE:
            return "-d";
        case CMD_SEARCH:
            return "-c";
        case CMD_NRLINES:
            return "-l";
        default:
            return "INVALID";
    }
}

