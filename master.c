#include "master.h"

pid_t workers[NUM_WORKERS];
int current_worker = 0;

int listen_fd_global;
proxy_config_t config_global;

void handle_sigchld(int sig) {
  pid_t pid;
  while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {

    for (int i = 0; i < NUM_WORKERS; i++) {
      if (workers[i] == pid) {
        printf("Worker %d crashed. Restarting...\n", pid);
        pid_t new_pid = fork();
        if (new_pid == 0) {

          worker_main(listen_fd_global, &config_global, i);
          exit(0);
        } else if (new_pid > 0) {
          workers[i] = new_pid;
          printf("Worker %d restarted as %d\n", pid, new_pid);
        } else {
          perror("Fork failed");
          exit(EXIT_FAILURE);
        }
        break;
      }
    }
  }
}

int create_listening_socket(const char *ip, int port) {
  int listen_fd;
  struct sockaddr_in addr;
  int opt = 1;

  if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }

  if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0) {
    perror("Invalid address");
    exit(EXIT_FAILURE);
  }

  if (bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("Bind failed");
    exit(EXIT_FAILURE);
  }

  if (listen(listen_fd, SOMAXCONN) < 0) {
    perror("Listen failed");
    exit(EXIT_FAILURE);
  }

  return listen_fd;
}

void fork_workers() {
  for (int i = 0; i < NUM_WORKERS; i++) {
    pid_t pid = fork();
    if (pid == 0) {

      worker_main(listen_fd_global, &config_global, i);
      exit(0);
    } else if (pid > 0) {
      workers[i] = pid;
      printf("Forked worker %d with PID %d\n", i, pid);
    } else {
      perror("Fork failed");
      exit(EXIT_FAILURE);
    }
  }
}