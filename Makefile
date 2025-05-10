# Compiler
CC = gcc

# Directories
SRC_DIR = src
DOC_DIR = $(SRC_DIR)/document
INC_DIR = include
BIN_DIR = bin

# Source files
CLIENT_SRC = $(SRC_DIR)/client.c
SERVER_SRC = $(SRC_DIR)/server.c
UTILS_SRC = $(SRC_DIR)/utils.c
CMD_SRC = $(SRC_DIR)/commandRunner.c
CACHE_SRC = $(SRC_DIR)/cache.c
DOC_SRCS = $(DOC_DIR)/document.c $(DOC_DIR)/documentManager.c

# Executables
CLIENT_EXEC = $(BIN_DIR)/dclient
SERVER_EXEC = $(BIN_DIR)/dserver

# Include directories
INCLUDES = -I$(INC_DIR) -I$(INC_DIR)/document

# Compiler flags
CFLAGS = -Wall -Wextra -std=gnu11 -g `pkg-config --cflags glib-2.0` $(INCLUDES)
LIBS = `pkg-config --libs glib-2.0`

# Targets
all: $(CLIENT_EXEC) $(SERVER_EXEC)

$(CLIENT_EXEC): $(CLIENT_SRC) $(UTILS_SRC) $(CMD_SRC) $(CACHE_SRC) $(DOC_SRCS)
	@mkdir -p $(BIN_DIR) # Create bin directory if it doesnt exist
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) `pkg-config --libs glib-2.0`

$(SERVER_EXEC): $(SERVER_SRC) $(UTILS_SRC) $(CMD_SRC) $(CACHE_SRC) $(DOC_SRCS)
	@mkdir -p $(BIN_DIR) # Create bin directory if it doesnt exist
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) `pkg-config --libs glib-2.0`

clean:
	rm -f $(BIN_DIR)/*

.PHONY: all clean