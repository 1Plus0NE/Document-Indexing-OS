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

typedef struct msg{
    char info[64];
    int pid;
} Msg;

#endif