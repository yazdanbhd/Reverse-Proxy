#include "shared-logging.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define SHARED_LOG_NAME "/proxy_log_buffer"

shared_log_buffer_t *log_buffer = NULL;

int init_shared_log_buffer() {
  int shm_fd = shm_open(SHARED_LOG_NAME, O_CREAT | O_RDWR, 0666);
  if (shm_fd == -1) {
    perror("shm_open failed");
    return -1;
  }

  if (ftruncate(shm_fd, sizeof(shared_log_buffer_t)) == -1) {
    perror("ftruncate failed");
    close(shm_fd);
    return -1;
  }

  log_buffer = mmap(NULL, sizeof(shared_log_buffer_t), PROT_READ | PROT_WRITE,
                    MAP_SHARED, shm_fd, 0);
  if (log_buffer == MAP_FAILED) {
    perror("mmap failed");
    close(shm_fd);
    return -1;
  }

  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
  pthread_mutex_init(&log_buffer->mutex, &attr);
  pthread_mutexattr_destroy(&attr);

  log_buffer->next_write_index = 0;
  log_buffer->total_logs = 0;

  close(shm_fd);
  return 0;
}

void add_log_entry(const char *client_ip, const char *status) {
  if (!log_buffer) {
    fprintf(stderr, "Log buffer not initialized\n");
    return;
  }

  pthread_mutex_lock(&log_buffer->mutex);

  log_entry_t *entry = &log_buffer->logs[log_buffer->next_write_index];
  entry->timestamp = time(NULL);
  strncpy(entry->client_ip, client_ip, MAX_IP_LEN - 1);
  strncpy(entry->status, status, MAX_STATUS_LEN - 1);
  entry->worker_pid = getpid();

  log_buffer->next_write_index = (log_buffer->next_write_index + 1) % MAX_LOGS;
  pthread_mutex_unlock(&log_buffer->mutex);
}

int read_logs(log_entry_t *output_logs, int max_logs) {
  if (!log_buffer) {
    fprintf(stderr, "Log buffer not initialized\n");
    return -1;
  }

  pthread_mutex_lock(&log_buffer->mutex);
  int start_index = log_buffer->next_write_index; // bug: incorrect calculation
  int count = 0;

  for (int i = start_index; i < log_buffer->total_logs; i++) {
    if (count >= max_logs)
      break;
    output_logs[count++] = log_buffer->logs[i];
  }

  pthread_mutex_unlock(&log_buffer->mutex);
  return count;
}
