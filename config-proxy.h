#ifndef CONFIG_H
#define CONFIG_H

#include <netinet/in.h>

#define MAX_HOST_LEN 256
#define MAX_PORT_LEN 16

typedef struct {
  char inbound_ip[INET_ADDRSTRLEN];
  u_int8_t inbound_port;
  char outbound_host[MAX_HOST_LEN];
  u_int8_t outbound_port;
} proxy_config_t;

void parse_arguments(int argc, char *argv[], proxy_config_t *config);

#endif // CONFIG_H