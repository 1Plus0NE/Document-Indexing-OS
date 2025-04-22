#include "../include/utils.h"

int validateFields(char* title, char* authors, char* path){
    if ((strlen(title) > TITLE_SIZE) ||
        (strlen(authors) > AUTHORS_SIZE) ||
        (strlen(path) > PATH_SIZE))
        return 0;
    
    return 1;
}

CommandType parseCommand(char* command){
    if(strcmp(command, "-a") == 0) return CMD_INDEX;
    if(strcmp(command, "-d") == 0) return CMD_REMOVE;
    if(strcmp(command, "-c") == 0) return CMD_SEARCH;
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
        default:
            return "INVALID";
    }
}

