#define _POSIX_C_SOURCE 200809L

#include "config-proxy.h"
#include "master.h"
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  parse_arguments(argc, argv, &config_global);

  listen_fd_global = create_listening_socket(config_global.inbound_ip,
                                             config_global.inbound_port);
  printf("Proxy listening on %s:%d, forwarding to %s\n",
         config_global.inbound_ip, config_global.inbound_port,
         config_global.outbound_host);

  struct sigaction sa;
  sa.sa_handler = handle_sigchld;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
  if (sigaction(SIGCHLD, &sa, 0) == -1) {
    perror("sigaction");
    return 1;
  }

  fork_workers();

  while (1) {
    pause();
  }

  close(listen_fd_global);
  return 0;
}
