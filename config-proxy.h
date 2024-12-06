#ifndef CONFIG_PROXY_H
#define CONFIG_PROXY_H

#include <netinet/in.h>

#define MAX_HOST_LEN 256
#define MAX_PORT_LEN 256

typedef struct {
  char inbound_ip[INET_ADDRSTRLEN];
  u_int16_t inbound_port;
  char outbound_host[MAX_HOST_LEN];
  u_int16_t outbound_port;
} proxy_config_t;

void parse_arguments(int argc, char *argv[], proxy_config_t *config);

#endif // CONFIG_PROXY_H