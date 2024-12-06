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

void set_cpu_affinity(int cpu_id);

int set_nonblocking(int fd);

void handle_client(int client_fd, const proxy_config_t *config);

int worker_main(int listen_fd, proxy_config_t *config, int cpu_id);

#endif // WORKER_H.