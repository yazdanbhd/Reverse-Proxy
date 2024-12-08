#include "../include/config-proxy.h"
#include <getopt.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void parse_arguments(int argc, char *argv[], proxy_config_t *config) {
  static struct option long_options[] = {
      {"inbound", required_argument, 0, 'i'},
      {"outbound", required_argument, 0, 'o'},
      {0, 0, 0, 0}};
  int option_index = 0;
  int c;

  // Default values
  strncpy(config->inbound_ip, "0.0.0.0", INET_ADDRSTRLEN - 1);
  config->inbound_ip[INET_ADDRSTRLEN - 1] = '\0'; // Ensure null-termination
  config->inbound_port = 80;                      // Default inbound port
  strncpy(config->outbound_host, "localhost",
          MAX_HOST_LEN - 1);                      // Default outbound host
  config->outbound_host[MAX_HOST_LEN - 1] = '\0'; // Ensure null-termination
  config->outbound_port = 8080;                   // Default outbound port

  while ((c = getopt_long(argc, argv, "i:o:", long_options, &option_index)) !=
         -1) {
    switch (c) {
    case 'i': {
      // Parse inbound IP and port
      char *colon = strchr(optarg, ':');
      if (colon) {
        *colon = '\0';
        strncpy(config->inbound_ip, optarg, INET_ADDRSTRLEN - 1);
        config->inbound_ip[INET_ADDRSTRLEN - 1] =
            '\0'; // Ensure null-termination
        config->inbound_port = atoi(colon + 1);
      } else {
        // If no port is provided, use default
        strncpy(config->inbound_ip, optarg, INET_ADDRSTRLEN - 1);
        config->inbound_ip[INET_ADDRSTRLEN - 1] =
            '\0'; // Ensure null-termination
      }
      break;
    }
    case 'o': {
      // Parse outbound host and port
      char *colon = strchr(optarg, ':');
      if (colon) {
        *colon = '\0';
        strncpy(config->outbound_host, optarg, MAX_HOST_LEN - 1);
        config->outbound_host[MAX_HOST_LEN - 1] =
            '\0'; // Ensure null-termination
        config->outbound_port = atoi(colon + 1);
      } else {
        // If no port is provided, use default
        strncpy(config->outbound_host, optarg, MAX_HOST_LEN - 1);
        config->outbound_host[MAX_HOST_LEN - 1] =
            '\0'; // Ensure null-termination
      }
      break;
    }
    default:
      fprintf(stderr, "Usage: ./proxy-server --inbound 127.0.0.1:3000 "
                      "--outbound localhost:8080\n");
      exit(EXIT_FAILURE);
    }
  }

  if (strlen(config->outbound_host) == 0) {
    fprintf(stderr, "Outbound host is required.\n");
    exit(EXIT_FAILURE);
  }
}
