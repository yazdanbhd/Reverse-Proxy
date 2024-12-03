#include "config.h"
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

  strncpy(config->inbound_ip, "0.0.0.0", INET_ADDRSTRLEN - 1);
  config->inbound_ip[INET_ADDRSTRLEN - 1] = '\0';
  config->inbound_port = 80;
  strncpy(config->outbound_host, "localhost", MAX_HOST_LEN - 1);
  config->outbound_host[MAX_HOST_LEN - 1] = '\0';
  config->outbound_port = 8080;

  while ((c = getopt_long(argc, argv, "i:o:", long_options, &option_index)) !=
         -1) {
    switch (c) {
    case 'i': {
      char *colon = strchr(optarg, ':');
      if (colon) {
        *colon = '\0';
        strncpy(config->inbound_ip, optarg, INET_ADDRSTRLEN - 1);
        config->inbound_ip[INET_ADDRSTRLEN - 1] = '\0';
        config->inbound_port = atoi(colon + 1);
      } else {
        strncpy(config->inbound_ip, optarg, INET_ADDRSTRLEN - 1);
        config->inbound_ip[INET_ADDRSTRLEN - 1] = '\0';
      }
      break;
    }
    case 'o': {
      char *colon = strchr(optarg, ':');
      if (colon) {
        *colon = '\0';
        strncpy(config->outbound_host, optarg, MAX_HOST_LEN - 1);
        config->outbound_host[MAX_HOST_LEN - 1] = '\0';
        config->outbound_port = atoi(colon + 1);
      } else {
        strncpy(config->outbound_host, optarg, MAX_HOST_LEN - 1);
        config->outbound_host[MAX_HOST_LEN - 1] = '\0';
      }
      break;
    }
    default:
      exit(EXIT_FAILURE);
    }
  }
}
