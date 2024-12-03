#ifndef SHARED_LOGGING_H
#define SHARED_LOGGING_H

#include <netinet/in.h>
#include <pthread.h>
#include <time.h>

#define MAX_LOGS 1000
#define MAX_IP_LEN 46
#define MAX_STATUS_LEN 50

typedef struct {
  time_t timestamp;
  char client_ip[MAX_IP_LEN];
  pid_t worker_pid;
  char status[MAX_STATUS_LEN];
} log_entry_t;

typedef struct {
  log_entry_t logs[MAX_LOGS];
  int next_write_index;
  int total_logs;
  pthread_mutex_t mutex;
} shared_log_buffer_t;

int init_shared_log_buffer();
void add_log_entry(const char *client_ip, const char *status);
int read_logs(log_entry_t *output_logs, int max_logs);

#endif // SHARED_LOGGING_H