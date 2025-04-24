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
    CMD_INDEX,   // -a
    CMD_REMOVE,  // -d
    CMD_SEARCH,  // -c
    CMD_NRLINES, // -l
    CMD_IDLIST,  // -s
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
 * Verifies if a given directory path exists
 * 
 * @param dir_path the directory's path
 * 
 * @return 1 if it exists, 0 otherwise
 */
int verifyDirectory(char* dir_path);

/**
 * Creates a FIFO with the given name
 * 
 * @param name the FIFO's name
 * 
 * @return 1 if FIFO was created with success, 0 otherwise
 */
int createFIFO(char* name);

/**
 * Opens a FIFO with the given name
 * 
 * @param name the FIFO's name
 * @param mode the mode to open the FIFO, eg. O_RDONLY, O_WRONLY
 * 
 * @return the file descritptor of the FIFO if it was opened with success, 0 otherwise
 */
int openFIFO(char* name, int mode);

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
 *          - CMD_INDEX   for "-a"
 *          - CMD_REMOVE  for "-d"
 *          - CMD_SEARCH  for "-c"
 *          - CMD_NRLINES for "-l"
 *          - CMD_IDLIST  for "-s"
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
 *          - "-l" for CMD_NRLINES
 *          - "-s" for CMD_IDLIST
 *          - "INVALID" for CMD_INVALID
 */
char* commandTypeToString(CommandType cmd);

/**
 * Validates given fields for a command request and builds an error message if an error is handled
 * 
 * @param argc the number of command-line arguments
 * @param argv the array of command-lines arguments
 * @param msg the command request structure
 * @param client_fifo the client's fifo name, used for cleanup if needed
 * 
 * @return 1 if the validation succeeds and the message is build, 0 on error
 */
int validateAndBuildMessage(int argc, char* argv[], Msg* msg, char* client_fifo);

#endif