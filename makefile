# Compiler and Flags
CC = gcc
CFLAGS = -Wall -O2

# Directories
SRC_DIR = src
OBJ_DIR = obj
INCLUDE_DIR = include

# Source files
SRC_FILES = main.c master.c worker.c config-proxy.c shared-logging.c collect-logs.c

# Object files (in OBJ_DIR)
OBJ_FILES = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

# Header files
HEADERS = master.h config-proxy.h worker.h shared-logging.h collect-logs.h

# Final executable
EXEC = proxy-server

# Ensure obj directory exists
$(shell mkdir -p $(OBJ_DIR))

# Default target
all: $(EXEC)

# Link object files into the final executable
$(EXEC): $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS:%=$(INCLUDE_DIR)/%)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Clean up object files and executable
clean:
	rm -rf $(OBJ_DIR) $(EXEC)

.PHONY: all clean