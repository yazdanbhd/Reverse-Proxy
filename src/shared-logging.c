#define _GNU_SOURCE
#include "../include/shared-logging.h"
#include <errno.h>
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

  // set the size of the shared memory
  if (ftruncate(shm_fd, sizeof(shared_log_buffer_t)) == -1) {
    perror("ftruncate failed");
    close(shm_fd);
    return -1;
  }

  // map the shared memory
  log_buffer = mmap(NULL, sizeof(shared_log_buffer_t), PROT_READ | PROT_WRITE,
                    MAP_SHARED, shm_fd, 0);

  if (log_buffer == MAP_FAILED) {
    perror("mmap failed");
    close(shm_fd);
    return -1;
  }

  // initialize mutex attributes for process-shared mutex
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);

  // initialize the mutex
  pthread_mutex_init(&log_buffer->mutex, &attr);
  pthread_mutexattr_destroy(&attr);

  // initialize other buffer properties
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

  // add new log
  log_entry_t *entry = &log_buffer->logs[log_buffer->next_write_index];

  entry->timestamp = time(NULL);
  strncpy(entry->client_ip, client_ip, MAX_IP_LEN - 1);
  entry->client_ip[MAX_IP_LEN - 1] = '\0';
  strncpy(entry->status, status, MAX_STATUS_LEN - 1);
  entry->status[MAX_STATUS_LEN - 1] = '\0';
  entry->worker_pid = getpid();

  // update with circular buffer
  log_buffer->next_write_index = (log_buffer->next_write_index + 1) % MAX_LOGS;

  // track logs
  if (log_buffer->total_logs < MAX_LOGS) {
    log_buffer->total_logs++;
  }

  pthread_mutex_unlock(&log_buffer->mutex);
}

int read_logs(log_entry_t *output_logs, int max_logs) {
  if (!log_buffer) {
    fprintf(stderr, "Log buffer not initialized\n");
    return -1;
  }

  pthread_mutex_lock(&log_buffer->mutex);

  // determine how many logs to return
  int logs_to_return =
      (log_buffer->total_logs < max_logs) ? log_buffer->total_logs : max_logs;

  // copy logs, starting from the oldest entry
  int start_index =
      (log_buffer->next_write_index - log_buffer->total_logs + MAX_LOGS) %
      MAX_LOGS;

  for (int i = 0; i < logs_to_return; i++) {
    int index = (start_index + i) % MAX_LOGS;
    memcpy(&output_logs[i], &log_buffer->logs[index], sizeof(log_entry_t));
  }

  // reset logs after reading once
  log_buffer->total_logs = 0;
  log_buffer->next_write_index = 0;

  pthread_mutex_unlock(&log_buffer->mutex);

  return logs_to_return;
}