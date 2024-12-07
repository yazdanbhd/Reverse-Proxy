#include "worker.h"

void set_cpu_affinity(int cpu_id) {
  cpu_set_t set;
  CPU_ZERO(&set);
  CPU_SET(cpu_id, &set);
  if (sched_setaffinity(0, sizeof(cpu_set_t), &set) < 0) {
    perror("sched_setaffinity failed");
  }
}

int set_nonblocking(int fd) {
  int flags;
  if ((flags = fcntl(fd, F_GETFL, 0)) < 0) {
    flags = 0;
  }
  return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int validate_host_header(const char *buffer, const proxy_config_t *config) {
  char *host_start = strcasestr(buffer, HOST_HEADER_PREFIX);
  if (!host_start) {
    return 0; // no host header found
  }

  // move past "Host: " prefix
  host_start += strlen(HOST_HEADER_PREFIX);

  // find end of host header
  char *host_end = strpbrk(host_start, "\r\n");
  if (host_end) {
    *host_end = '\0'; // null-terminate the host
  }

  while (*host_start == ' ')
    host_start++;
  if (host_end) {
    while (host_end > host_start && (*(host_end - 1) == ' ')) {
      host_end--;
    }
  }

  // compare with configured outbound host
  if (strncasecmp(host_start, config->outbound_host,
                  strlen(config->outbound_host)) != 0) {
    return 0; // host header didnt match
  }

  return 1; // host header is valid
}

void send_bad_request(int client_fd) {
  const char *bad_request_response = "HTTP/1.1 400 Bad Request\r\n"
                                     "Content-Type: text/plain\r\n"
                                     "Content-Length: 11\r\n"
                                     "\r\n"
                                     "Bad Request";

  send(client_fd, bad_request_response, strlen(bad_request_response), 0);
}

void handle_client(int client_fd, const proxy_config_t *config) {
  char client_ip[INET_ADDRSTRLEN] = "unknown";
  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);

  // get client IP address
  if (getpeername(client_fd, (struct sockaddr *)&client_addr,
                  &client_addr_len) == 0) {
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
  }

  printf("Worker %d handling client %d from %s\n", getpid(), client_fd,
         client_ip);

  // read the HTTP request from the client
  char buffer[BUFFER_SIZE];
  int bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
  if (bytes_read <= 0) {
    perror("Failed to read from client");
    close(client_fd);
    return;
  }
  buffer[bytes_read] = '\0';

  // handle /collect_logs endpoint
  if (strstr(buffer, "GET /.svc/collect_logs") != NULL) {
    int response_length;
    char *log_response = generate_log_collection_response(&response_length);
    if (log_response) {
      const char *header = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/plain\r\n"
                           "Content-Length: ";
      char content_length[16];
      snprintf(content_length, sizeof(content_length), "%d\r\n\r\n",
               response_length);

      send(client_fd, header, strlen(header), 0);
      send(client_fd, content_length, strlen(content_length), 0);
      send(client_fd, log_response, response_length, 0);

      free(log_response);
    } else {
      const char *error_response = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
      send(client_fd, error_response, strlen(error_response), 0);
    }
    close(client_fd);
    return;
  }

  // validate Host header
  if (!validate_host_header(buffer, config)) {
    printf("Invalid host header from %s\n", client_ip);
    add_log_entry(client_ip, "400 Bad Request - Invalid Host Header");
    send_bad_request(client_fd);
    close(client_fd);
    return;
  }

  add_log_entry(client_ip, "Valid Request");

  // parse outbound host and port
  char host[256];
  int port = config->outbound_port; // default to http
  if (strchr(config->outbound_host, ':')) {
    if (sscanf(config->outbound_host, "%255[^:]:%d", host, &port) != 2) {
      fprintf(stderr, "Invalid outbound host format: %s\n",
              config->outbound_host);
      close(client_fd);
      return;
    }
  } else {
    strncpy(host, config->outbound_host, sizeof(host) - 1);
    host[sizeof(host) - 1] = '\0';
  }
  printf("Parsed outbound host: %s, port: %d\n", host, port);

  // resolve the upstream host
  struct hostent *he = gethostbyname(host);
  if (!he) {
    fprintf(stderr, "DNS resolution failed for host: %s\n", host);
    perror("gethostbyname");
    close(client_fd);
    return;
  }

  // connect to the upstream server
  int upstream_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (upstream_fd < 0) {
    perror("Failed to create upstream socket");
    close(client_fd);
    return;
  }

  struct sockaddr_in upstream_addr;
  upstream_addr.sin_family = AF_INET;
  upstream_addr.sin_port = htons(port);
  upstream_addr.sin_addr = *((struct in_addr *)he->h_addr_list[0]);

  if (connect(upstream_fd, (struct sockaddr *)&upstream_addr,
              sizeof(upstream_addr)) < 0) {
    perror("Failed to connect to upstream server");
    close(client_fd);
    close(upstream_fd);
    return;
  }
  printf("Connected to upstream server at %s:%d\n", host, port);

  // forward request to the upstream server
  int bytes_sent = send(upstream_fd, buffer, bytes_read, 0);
  if (bytes_sent <= 0) {
    perror("Failed to send request to upstream server");
    close(client_fd);
    close(upstream_fd);
    return;
  }

  // relay response back to the client
  while ((bytes_read = recv(upstream_fd, buffer, sizeof(buffer), 0)) > 0) {
    send(client_fd, buffer, bytes_read, 0);
  }

  if (bytes_read == 0) {
    printf("Upstream server closed the connection\n");
  } else {
    perror("Failed to receive response from upstream server");
  }

  close(upstream_fd);
  close(client_fd);
  printf("Worker %d closed connection %d\n", getpid(), client_fd);
}

int worker_main(int listen_fd, proxy_config_t *config, int cpu_id) {
  init_shared_log_buffer();

  printf("Worker %d started on CPU %d\n", getpid(), cpu_id);

  // set CPU affinity
  set_cpu_affinity(cpu_id);

  // create an epoll instance
  int epoll_fd = epoll_create1(0);
  if (epoll_fd == -1) {
    perror("epoll_create1 failed");
    exit(EXIT_FAILURE);
  }

  // add the listening socket to the epoll instance
  struct epoll_event event;
  event.events = EPOLLIN;
  event.data.fd = listen_fd;
  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &event) == -1) {
    perror("epoll_ctl failed");
    exit(EXIT_FAILURE);
  }

  // allocate memory for epoll events
  struct epoll_event *events = calloc(MAX_EVENTS, sizeof(struct epoll_event));
  if (!events) {
    perror("Failed to allocate memory for epoll events");
    exit(EXIT_FAILURE);
  }

  // event loop
  while (1) {
    int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
    if (n == -1) {
      perror("epoll_wait failed");
      continue;
    }

    for (int i = 0; i < n; i++) {
      if (events[i].data.fd == listen_fd) {
        // accept a new client connection
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd =
            accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == -1) {
          perror("Failed to accept client connection");
          continue;
        }

        set_nonblocking(client_fd);
        event.events = EPOLLIN | EPOLLET;
        event.data.fd = client_fd;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1) {
          perror("epoll_ctl failed for client");
          close(client_fd);
        }
        printf("Accepted new client connection %d\n", client_fd);

      } else {
        // handle client connection
        int client_fd = events[i].data.fd;
        handle_client(client_fd, config);
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
      }
    }
  }

  free(events);
  close(epoll_fd);
  return 0;
}