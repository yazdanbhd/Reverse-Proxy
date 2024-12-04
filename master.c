#include "master.h"

pid_t workers[NUM_WORKERS];
int listen_fd_global;
proxy_config_t config_global;

void handle_sigchld(int sig) { printf("SIGCHLD received. \n"); }

int create_listening_socket(const char *ip, int port) {
  printf("test listening socket.\n");
  return -1;
}

void fork_workers() { printf("tets worker.\n"); }

int main(int argc, char *argv[]) {
  printf("master process.\n");
  return 0;
}
