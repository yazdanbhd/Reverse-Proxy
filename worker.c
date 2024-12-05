#include "worker.h"

void set_cpu_affinity(int cpu_id) {
  printf("setting CPU affinity (CPU: %d).\n", cpu_id);
}

int set_nonblocking(int fd) {
  printf("setting non-blocking mode.\n");
  return fd; // No-op
}

int worker_main(int listen_fd, proxy_config_t *config, int cpu_id) {
  printf("Worker process (PID: %d) started on CPU %d\n", getpid(), cpu_id);
  return 0;
}
