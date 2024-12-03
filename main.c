#include "shared-logging.h"

int main(int argc, char *argv[]) {
  init_shared_log_buffer();
  add_log_entry("127.0.0.1", "Test Log Entry");
  log_entry_t logs[MAX_LOGS];
  int count = read_logs(logs, MAX_LOGS);
  printf("Read %d logs\n", count);

  return 0;
}
