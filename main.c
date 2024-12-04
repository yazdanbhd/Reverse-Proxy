#include "collect-logs.h"
#include "config-proxy.h"
#include "shared-logging.h"
#include <stdio.h>

proxy_config_t config_global;

int main(int argc, char *argv[]) {
  parse_arguments(argc, argv, &config_global);
  printf("Inbound: %s:%d, Outbound: %s:%d\n", config_global.inbound_ip,
         config_global.inbound_port, config_global.outbound_host,
         config_global.outbound_port);

  if (init_shared_log_buffer() != 0) {
    fprintf(stderr, "Failed to initialize shared log buffer\n");
    return 1;
  }
  add_log_entry("127.0.0.1", "Test Log 1");
  add_log_entry("127.0.0.2", "Test Log 2");

  int response_length;
  char *response = generate_log_collection_response(&response_length);
  if (response) {
    printf("Log Collection Response:\n%s", response);
    free(response);
  } else {
    fprintf(stderr, "Failed to generate log collection response\n");
  }

  return 0;
}
