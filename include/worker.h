#ifndef WORKER_H
#define WORKER_H

#define _GNU_SOURCE

#include "collect-logs.h"
#include "config-proxy.h"
#include "shared-logging.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define MAX_EVENTS 1000
#define BUFFER_SIZE 4096
#define HOST_HEADER_PREFIX "Host: "

void set_cpu_affinity(int cpu_id); // function to bind process to a specific CPU

int set_nonblocking(int fd); // function to set file descriptor as non-blocking

int validate_host_header(
    const char *buffer,
    const proxy_config_t *config); // function to validate host header
void send_bad_request(
    int client_fd); // function to send and handle 400 Bad Request response
void handle_client(
    int client_fd,
    const proxy_config_t *config); // function to handle client connection and
                                   // forward to the upstream server
int worker_main(int listen_fd, proxy_config_t *config,
                int cpu_id); // main worker process

#endif // WORKER_H.