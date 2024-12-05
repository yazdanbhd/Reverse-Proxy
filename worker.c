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
  set_cpu_affinity(cpu_id);

  int epoll_fd = epoll_create1(0);
  if (epoll_fd == -1) {
    perror("epoll_create1 failed");
    exit(EXIT_FAILURE);
  }

  struct epoll_event event;
  event.events = EPOLLIN;
  event.data.fd = listen_fd;
  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &event) == -1) {
    perror("epoll_ctl failed");
    exit(EXIT_FAILURE);
  }

  struct epoll_event events[MAX_EVENTS];
  while (1) {
    int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
    for (int i = 0; i < n; i++) {
      if (events[i].data.fd == listen_fd) {
        int client_fd = accept(listen_fd, NULL, NULL);
        if (client_fd == -1) {
          perror("accept failed");
          continue;
        }
        set_nonblocking(client_fd);
        event.events = EPOLLIN | EPOLLET;
        event.data.fd = client_fd;
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event);
      } else {
        handle_client(events[i].data.fd, config);
      }
    }
  }

  close(epoll_fd);
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
