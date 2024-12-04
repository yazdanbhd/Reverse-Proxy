#include "config-proxy.h"
#include "master.h"
#include <stdio.h>

proxy_config_t config_global;

int main(int argc, char *argv[]) {
  parse_arguments(argc, argv, &config_global);

  int listen_fd = create_listening_socket(config_global.inbound_ip,
                                          config_global.inbound_port);
  if (listen_fd < 0) {
    fprintf(stderr, "Failed to create listening socket\n");
    return 1;
  }

  printf("Listening socket created successfully on %s:%d\n",
         config_global.inbound_ip, config_global.inbound_port);

  return 0;
}
