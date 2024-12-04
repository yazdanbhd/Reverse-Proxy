#include "collect-logs.h"
#include <stdlib.h>
#include <string.h>

char *format_log_entry(const log_entry_t *entry) {
  char *formatted_entry = malloc(256);
  if (!formatted_entry)
    return NULL;

  snprintf(formatted_entry, 256, "%ld %s %d %s\n", entry->timestamp,
           entry->client_ip, entry->worker_pid, entry->status);

  return formatted_entry;
}

char *generate_log_collection_response(int *response_length) {
  log_entry_t logs[MAX_LOGS];
  int num_logs = read_logs(logs, MAX_LOGS);

  if (num_logs <= 0) {
    *response_length = 0;
    return strdup("No logs available\n");
  }

  size_t total_size;
  char **formatted_logs = malloc(num_logs * 2);
  if (!formatted_logs) {
    *response_length = 0;
    return NULL;
  }

  for (int i = 0; i < num_logs; i++) {
    formatted_logs[i] = format_log_entry(&logs[i]);
    if (formatted_logs[i]) {
      total_size += strlen(formatted_logs[i]);
    }
  }

  char *response;
  if (!response) {
    for (int i = 0; i < num_logs; i++) {
      free(formatted_logs[i]);
    }
    free(formatted_logs);
    *response_length = 0;
    return NULL;
  }

  response[0] = '\0';
  for (int i = 0; i < num_logs; i++) {
    if (formatted_logs[i]) {
      strcat(response, formatted_logs[i]);
      free(formatted_logs[i]);
    }
  }

  *response_length = total_size;
  return response;
}