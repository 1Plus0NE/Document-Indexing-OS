# Compiler
CC = gcc

# Directories
SRC_DIR = src
INC_DIR = include
BIN_DIR = bin

# Source files
CLIENT_SRC = $(SRC_DIR)/client.c
SERVER_SRC = $(SRC_DIR)/server.c

# Executables
CLIENT_EXEC = $(BIN_DIR)/client
SERVER_EXEC = $(BIN_DIR)/server

# Include directories
INCLUDES = -I$(INC_DIR)

# Compiler flags
CFLAGS = -Wall -Wextra $(INCLUDES)

# Targets
all: $(CLIENT_EXEC) $(SERVER_EXEC)

$(CLIENT_EXEC): $(CLIENT_SRC) $(INC_DIR)/utils.h
	@mkdir -p $(BIN_DIR)  # Create bin directory if it doesn't exist
	$(CC) $(CFLAGS) -o $@ $<

$(SERVER_EXEC): $(SERVER_SRC) $(INC_DIR)/utils.h
	@mkdir -p $(BIN_DIR)  # Create bin directory if it doesn't exist
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(BIN_DIR)/*

.PHONY: all clean