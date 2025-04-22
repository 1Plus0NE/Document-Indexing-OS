#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#ifndef UTILS_H
#define UTILS_H

// Define client's fifo path and name
#define CLIENT "client_fifo"

// Define server's fifo path and name
#define SERVER "server_fifo"

// Defines the max size for the following fields
#define TITLE_SIZE   200
#define AUTHORS_SIZE 200
#define PATH_SIZE    64

// Structure to represent a type of a command
typedef enum commandType{
    CMD_INDEX,  // -a
    CMD_REMOVE, // -d
    CMD_SEARCH, // -c
    CMD_INVALID 
}CommandType;

// Structure to represent a command to run
typedef struct msg{
    CommandType cmdType;
    char info[512];
    int pid;
    char response[512];
}Msg;

/**
 * Validates fields if they exceed a specific size
 *  
 * @param title title of a document
 * @param authors authors of a document
 * @param path local document's directory
 * 
 * @return 1 if fields do not exceed a size, 0 otherwise
 */
int validateFields(char* title, char* authors, char* path);

/**
 * Parses a command-line argument and returns the corresponding command type
 *
 * @param command The string representing the user's command (e.g., "-a", "-d", "-c")
 *
 * @returns The corresponding CommandType value:
 *          - CMD_INDEX for "-a"
 *          - CMD_REMOVE for "-d"
 *          - CMD_SEARCH for "-c"
 *          - CMD_INVALID if the command is not recognized
 */
CommandType parseCommand(char* command);

/**
 * Convert a command-type to the corresponding command-line
 *
 * @param cmd The commandType representing a user's command (e.g, CMD_INDEX, CMD_REMOVE, CMD_SEARCH)
 *
 * @returns The corresponding command-line:
 *          - "-a" for CMD_INDEX
 *          - "-d" for CMD_REMOVE
 *          - "-c" for CMD_SEARCH
 *          - "INVALID" for CMD_INVALID
 */
char* commandTypeToString(CommandType cmd);
#endif