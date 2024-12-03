#include "config-proxy.h"
#include <stdio.h>

proxy_config_t config_global;

int main(int argc, char *argv[]) {
  parse_arguments(argc, argv, &config_global);
  printf("Inbound: %s:%d, Outbound: %s:%d\n", config_global.inbound_ip,
         config_global.inbound_port, config_global.outbound_host,
         config_global.outbound_port);
  return 0;
}
