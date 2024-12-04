#ifndef MASTER_H
#define MASTER_H

#include "config-proxy.h"
#include "worker.h"

#define NUM_WORKERS 4
void handle_sigchld(int sig);
int create_listening_socket(const char *ip, int port);
void fork_workers();

#endif // MASTER_H
