# Reverse Proxy Server

## Introduction


This document is about my **Proxy Server** project. It is a reverse proxy written in the C . A reverse proxy is a server that takes requests from clients, sends them to another server (called an upstream server), and then sends the response back to the clients. This project also keeps logs of all the requests to help monitor and debug the server.

## Features

- **Reverse Proxy Functionality**:
    
    - Listens for client requests on a specified IP address and port.
    - Forwards these requests to an upstream server.
    - Sends the responses from the upstream server back to the clients.
- **Logging System**:
    
    - Collects logs with details like the time of request, client IP address, and process ID.
    - Uses shared memory to store logs, allowing multiple worker processes to write logs safely.
- **Multiple Worker Processes**:
    
    - Uses several worker processes to handle multiple client connections at the same time.
    - Improves the server’s performance and ability to handle many requests.

- **Automatic Logging Collection**:
    
    - Provides an endpoint (`/collect_logs`) to retrieve and view the collected logs.
- **Concurrency:**
	- Simultaneous Processing**: The proxy server can handle many client requests at the same time without making clients wait.
	- **Efficient Resource Use**: By managing multiple requests at once, the server uses its resources better, making it more efficient.
- **Crash Resilient:**
	- **Automatic Restart**: If a worker process crashes, the master process notices and restarts it automatically.
	- **Keeps Server Running**: This ensures that the proxy server stays up and running even if some workers fail.
- **Single Process Multiple Requests:**
	- **Handles Multiple Requests**: Each worker process can manage many client requests at the same time.
	- **Non-Blocking Operations**: Uses techniques like non-blocking I/O to handle multiple requests without waiting for each one to finish before starting the next.
- **Host Validation:**
	- **Checks Host Headers**: Validates the `Host` header in incoming HTTP requests to ensure they match the configured outbound host.
	- **Security Measure**: Prevents unauthorized or malicious requests by ensuring only requests with valid `Host` headers are processed.
	- **Error Handling**: Sends a `400 Bad Request` response to clients if the `Host` header is invalid, helping to protect the server from unwanted traffic.

## Project Structure

All the `*.c` files are kept in the `/src` folder and all the `*.h` files are kept in the `/include` folder.

- **`main.c`**:
    
    - The main program that starts the proxy server.
    - Sets up listening sockets and manages worker processes.
- **`master.c`** and **`master.h`**:
    
    - Manage the worker processes.
    - Handle signals and restart workers if they crash.
- **`worker.c`** and **`worker.h`**:
    
    - Handle client connections.
    - Forward requests to the upstream server and relay responses back to clients.
- **`config-proxy.c`** and **`config-proxy.h`**:
    
    - Parse and manage the proxy server’s settings from command-line arguments.
    - Set up inbound and outbound IP addresses and ports.
- **`shared-logging.c`** and **`shared-logging.h`**:
    
    - Manage shared memory for logging.
    - Provide functions to add and read log entries.
- **`collect-logs.c`** and **`collect-logs.h`**:
    
    - Collect and format log entries into a response that can be sent to clients.
- **`Makefile`**:
    
    - Used to compile the project.
    - Contains rules to build the executable and clean up compiled files.
- **`README.md`**:
    
    - This document.

## Requirements

To work with this project, you need the following tools installed on your system:

- **GCC (GNU Compiler Collection)**:
    - To compile the C code.
- **Make**:
    - To use the Makefile for building the project.

### Installing Requirements on Ubuntu

You can install these tools using the following commands:

```bash
sudo apt update
sudo apt install gcc make libcunit1-dev
```

## Installation

Follow these steps to set up the project on your computer:

### 1. Clone the Repository

If the project is on GitHub or another platform, you can clone it using `git`. Replace `yourusername` with the actual username.

```bash
git clone https://github.com/yazdanbhd/Reverse-Proxy.git
cd Reverse-Proxy
```

### 2. Install Dependencies

Make sure you have all the required tools installed (`gcc`, `make`). Refer to the **Requirements** section for installation instructions.

## Build

Use the Makefile to compile the project. The Makefile helps automate the build process.

### Steps to Build

1. **Run Make**:
    
    In the project directory, run the following command:
    
    ```bash
    make
    ```
    
    This command will compile all the `.c` files and create the `proxy-server` executable.
    
2. **Check for Errors**:
    
    If there are no errors, you will see the `proxy-server` executable in your project folder. If you see errors, make sure all dependencies are installed and files are in the correct locations.
    

### Makefile Explained

Here is a brief explanation of the Makefile:

```Makefile
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
```

- **Variables**:
    
    - `CC`: The compiler to use (gcc).
    - `CFLAGS`: Compiler flags (`-Wall` for all warnings, `-O2` for optimization).
    - `SRC_DIR`, `OBJ_DIR`, `INCLUDE_DIR`: Directories for source files, object files, and headers.
    - `SRC_FILES`: List of source `.c` files.
    - `OBJ_FILES`: List of object `.o` files placed in `obj/` directory.
    - `HEADERS`: List of header `.h` files.
    - `EXEC`: Name of the final executable (`proxy-server`).
- **Targets**:
    
    - `all`: The default target that builds the executable.
    - `$(EXEC)`: Links all object files into the final executable.
    - `$(OBJ_DIR)/%.o`: Compiles each `.c` file into an `.o` file in the `obj/` directory.
    - `clean`: Removes all object files and the executable.

## Usage

After building the project, you can run the proxy server with your desired settings.

### Running the Proxy Server

Use the following command to start the proxy server:

```bash
./proxy-server --inbound 127.0.0.1:3000 --outbound localhost:8080
```

### Command-Line Options

- `--inbound`:
    
    - Sets the IP address and port where the proxy server listens for client requests.
    - **Format**: `IP:PORT` (e.g., `127.0.0.1:3000`).
    - **Default**: `0.0.0.0:80` (all IP addresses on port 80).
- `--outbound`:
    
    - Sets the host and port where the proxy server forwards the client requests.
    - **Format**: `HOST:PORT` (e.g., `localhost:8080`).
    - **Default**: `localhost:8080`.

### Example

Start the proxy server to listen on `127.0.0.1` at port `3000` and forward requests to `localhost` at port `8080`:

```bash
./proxy-server --inbound 127.0.0.1:3000 --outbound localhost:8080
```

This means:

- Clients connect to `127.0.0.1:3000`.
- The proxy server sends these requests to `localhost:8080`.
- Responses from `localhost:8080` are sent back to the clients.

## Configuration

The proxy server can be configured using command-line arguments. Here are the details:

### Inbound Configuration

- **Option**: `--inbound` or `-i`
- **Purpose**: Sets the IP address and port where the proxy server listens for incoming client requests.
- **Format**: `IP:PORT`
- **Example**: `--inbound 127.0.0.1:3000`

### Outbound Configuration

- **Option**: `--outbound` or `-o`
- **Purpose**: Sets the host and port where the proxy server forwards the client requests.
- **Format**: `HOST:PORT`
- **Example**: `--outbound localhost:8080`

### Default Settings

If you do not provide these options, the proxy server uses the following default settings:

- **Inbound**: `0.0.0.0:80`
    - Listens on all IP addresses at port `80`.
- **Outbound**: `localhost:8080`
    - Forwards requests to `localhost` at port `8080`.

### Changing Settings

You can change these settings by providing different values when starting the proxy server. For example:

```bash
./proxy-server --inbound 192.168.1.100:4000 --outbound example.com:9090
```

This command sets:

- **Inbound**: `192.168.1.100:4000`
- **Outbound**: `example.com:9090`

## Clean Up

To remove all the compiled files and the executable, use the `clean` target in the Makefile.

### Steps to Clean

1. **Run Make Clean**:
    
    In the project directory, run:
    
    ```bash
    make clean
    ```
    
    This command will delete all `.o` object files in the `obj` directory and the `proxy-server` executable.
    
2. **Verify Clean Up**:
    
    After running the clean command, check that the `obj` directory is empty and that the `proxy-server` executable is removed.
    

## Project Structure Details


### Source and Header Files

- **`main.c`**:
    
    - Starts the proxy server.
    - Parses command-line arguments to set up configurations.
    - Creates listening sockets based on the inbound settings.
    - Sets up signal handlers to manage worker processes.
    - Forks worker processes to handle client connections.
    - Keeps the master process running to listen for signals.
- **`master.c`** and **`master.h`**:
    
    - Manage worker processes.
    - Handle signals like `SIGCHLD` to detect when a worker process terminates.
    - Restart worker processes if they crash to ensure the proxy server remains available.
- **`worker.c`** and **`worker.h`**:
    
    - Handle individual client connections.
    - Read client requests and forward them to the upstream server.
    - Receive responses from the upstream server and send them back to the client.
    - Validate HTTP headers to ensure requests are correct.
    - Log request details for monitoring and debugging.
- **`config-proxy.c`** and **`config-proxy.h`**:
    
    - Parse command-line arguments provided when starting the proxy server.
    - Set up the proxy configuration, including inbound and outbound settings.
    - Provide default settings if no arguments are provided.
- **`shared-logging.c`** and **`shared-logging.h`**:
    
    - Manage a shared memory buffer to store log entries.
    - Use mutexes to ensure that multiple processes can write to the log safely.
    - Provide functions to add new log entries and read existing logs.
- **`collect-logs.c`** and **`collect-logs.h`**:
    
    - Collect logs from the shared memory buffer.
    - Format log entries into a readable string that can be sent to clients.
    - Provide a response when clients request log information via the `/collect_logs` endpoint.

### Makefile

- **Purpose**:
    
    - Automates the build process.
    - Compiles source files into object files.
    - Links object files to create the final executable.
    - Provides targets for building, testing, and cleaning the project.
- **Key Targets**:
    
    - `all`: Compiles the entire project.
    - `proxy-server`: Links all object files to create the executable.
    - `$(OBJ_DIR)/%.o`: Compiles each `.c` file into an `.o` file.
    - `clean`: Removes all compiled object files and the executable.
    - `test`: Compiles and runs unit tests.

**note:** I wanted to use `Doxygen` for making a powerful documentation but i couldnt do that because of the lack of time, so if there is any chance, i can bring doxygen document in 1 or 2 extra days.