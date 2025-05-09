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
    if(strcmp(command, "-s") == 0) return CMD_IDLIST;
    if(strcmp(command, "-f") == 0) return CMD_SHUTDOWN;
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
        case CMD_IDLIST:
            return "-s";
        case CMD_SHUTDOWN:
            return "-f";
        default:
            return "INVALID"; // never happens
    }
}

int validateAndBuildMessage(int argc, char* argv[], Msg* msg, char* client_fifo){
    msg->cmdType = parseCommand(argv[1]); // first argument
    switch(msg->cmdType){
        case CMD_INDEX:
            if(argc !=6){
                char* usage = "Incorrect usage!\nIndex Document: -a <title> <authors> <year> <doc_path>\n";
                write(1, usage, strlen(usage));
                unlink(client_fifo);
                return 0;
            }
            else if(!validateFields(argv[2], argv[3], argv[5])){
                char* warning = "Warning: one of the fields exceeds its max size!\n"
                                "Title MAX SIZE: 200 Bytes\n"
                                "Authors MAX SIZE: 200 Bytes\n"
                                "Path MAX SIZE: 64 Bytes.\n";
                write(1, warning, strlen(warning));
                unlink(client_fifo);
                return 0;		
            }
            snprintf(msg->info, sizeof(msg->info), "%s|%s|%s|%s", argv[2], argv[3], argv[4], argv[5]); // fields to index a document
            break;

        case CMD_SEARCH:
        case CMD_REMOVE:
            if(argc != 3){
                char* usage = "Incorrect usage!\nConsult Document: -c <id>\nRemove Document: -d <id>\n";
                write(1, usage, strlen(usage));
                unlink(client_fifo);
                return 0;
            }
            strncpy(msg->info, argv[2], sizeof(msg->info) - 1);
            msg->info[sizeof(msg->info) - 1] = '\0';
            break;
        
        case CMD_NRLINES:
            if(argc != 4){
                char* usage = "Incorrect usage!\nNr lines by keyword: -l <id> <keyword>\n";
                write(1, usage, strlen(usage));
                unlink(client_fifo);
                return 0;
            }
            snprintf(msg->info, sizeof(msg->info), "%s|%s", argv[2], argv[3]); // eg. 1|sky
            break;

        case CMD_IDLIST:
            if(argc < 3 || argc > 4){
                char* usage = "Incorrect usage!\nList IDs keyword: -s <keyword>\nList IDs keyword: -s <keyword> <nr_processes>\n";
                write(1, usage, strlen(usage));
                unlink(client_fifo);
                return 0;
            }
            if(argc == 3) snprintf(msg->info, sizeof(msg->info), "%s|1", argv[2]);
            else snprintf(msg->info, sizeof(msg->info), "%s|%s", argv[2], argv[3]);
            break;

        case CMD_SHUTDOWN:
            if(argc != 2){
                char* usage = "Incorrect usage!\nShutdown Request: -f\n";
                write(1, usage, strlen(usage));
                unlink(client_fifo);
                return 0;
            }
            strncpy(msg->info, argv[1], sizeof(msg->info) - 1);
            msg->info[sizeof(msg->info) - 1] = '\0';
            break;

        case CMD_INVALID:
        default:
            char* invalid = "Warning: invalid argument!\n"
                            "Consider the following commands:\n"
                            "INDEX: -a " "\"title\" " "\"authors\" " "\"year\" " "\"path\"\n"
                            "SEARCH: -c " "\"id\"\n"
                            "REMOVE: -d " "\"id\"\n"
                            "NR_LINES: -l " "\"id\" " "\"keyword\"\n";
            write(1, invalid, strlen(invalid));
            unlink(client_fifo);
            return 0;
    }
    return 1;
}