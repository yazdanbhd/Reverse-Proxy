# Reverse Proxy Server

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
    


### Contributing


- *Guidelines*
	- Use meaningful commit messages.
	- Follow the existing code style. (currently using clang-format for code formatting)
	- Add comments and documentation for new features.

- *Submitting Changes*
	1. Fork the repository.
	2. Create a new branch for your work.
	3. Open a pull request and describe your changes.

