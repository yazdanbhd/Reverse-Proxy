#define _POSIX_C_SOURCE 200809L

#include "config-proxy.h"
#include "master.h"
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  // parse command-line arguments
  parse_arguments(argc, argv, &config_global);

  // create the listening socket
  listen_fd_global = create_listening_socket(config_global.inbound_ip,
                                             config_global.inbound_port);
  printf("Proxy listening on %s:%d, forwarding to %s\n",
         config_global.inbound_ip, config_global.inbound_port,
         config_global.outbound_host);

  // setup signal handler for child process management
  struct sigaction sa;
  sa.sa_handler = handle_sigchld;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
  if (sigaction(SIGCHLD, &sa, 0) == -1) {
    perror("sigaction");
    return 1;
  }

  // fork worker processes
  fork_workers();

  // master process waits for siganals indefinitely
  while (1) {
    pause();
  }

  // cleanup
  close(listen_fd_global);
  return 0;
}
