#include "worker.h"

void set_cpu_affinity(int cpu_id) {
  cpu_set_t set;
  CPU_ZERO(&set);
  CPU_SET(cpu_id, &set);
  if (sched_setaffinity(0, sizeof(cpu_set_t), &set) < 0) {
    perror("sched_setaffinity failed");
  }
}

int set_nonblocking(int fd) {
  int flags;
  if ((flags = fcntl(fd, F_GETFL, 0)) < 0) {
    perror("fcntl F_GETFL failed");
    return -1;
  }
  if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
    perror("fcntl F_SETFL failed");
    return -1;
  }
  return fd;
}

int worker_main(int listen_fd, proxy_config_t *config, int cpu_id) {
  printf("Worker process (PID: %d) started on CPU %d\n", getpid(), cpu_id);
  return 0;
}

void handle_client(int client_fd, const proxy_config_t *config) {
  char buffer[BUFFER_SIZE];
  int bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

  if (bytes_read <= 0) {
    perror("Failed to read from client");
    close(client_fd);
    return;
  }

  buffer[bytes_read] = '\0';
  printf("Worker %d received: %s\n", getpid(), buffer);

  send(client_fd, buffer, bytes_read, 0);
  close(client_fd);
}
