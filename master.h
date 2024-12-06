#ifndef MASTER_H
#define MASTER_H

#include "config-proxy.h"
#include "worker.h"
#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#define NUM_WORKERS 4 // based on CPU cores

extern pid_t workers[NUM_WORKERS];
extern int current_worker;
extern int listen_fd_global;
extern proxy_config_t config_global;

void handle_sigchld(int sig);

int create_listening_socket(const char *ip, int port);

void fork_workers();

#endif // MASTER_H
