#include "master.h"

pid_t workers[NUM_WORKERS];
int listen_fd_global;
proxy_config_t config_global;

void handle_sigchld(int sig) { printf("SIGCHLD received. \n"); }

int create_listening_socket(const char *ip, int port) {
  int listen_fd;
  struct sockaddr_in addr;
  int opt = 1;

  if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Socket creation failed");
    return -1;
  }

  if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    perror("setsockopt failed");
    return -1;
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0) {
    perror("Invalid IP address");
    return -1;
  }

  if (bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("Bind failed");
    return -1;
  }

  if (listen(listen_fd, SOMAXCONN) < 0) {
    perror("Listen failed");
    return -1;
  }

  return listen_fd;
}

void fork_workers() {
  for (int i = 0; i < NUM_WORKERS; i++) {
    pid_t pid = fork();
    if (pid == 0) {
      printf("Worker process forked (PID: %d).\n", getpid());
      // TODO: Worker logic
      exit(0);
    } else if (pid > 0) {
      workers[i] = pid;
      printf("Forked worker %d with PID %d\n", i, pid);
    } else {
      perror("Fork failed");
      exit(1);
    }
  }
}

int main(int argc, char *argv[]) {
  printf("master process.\n");
  return 0;
}
