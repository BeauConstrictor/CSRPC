#include <sys/socket.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>

#include "server.h"

#define EOT '\x04'
#define BUFSIZE 128

const char *CSRPC_PATH = "/tmp/csrpc";

struct server {
  int listen_fd;
  int client_fd;
  char buf[BUFSIZE];
  size_t len;
};

static int create_server_socket(const char *path) {
  int fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (fd < 0) {
      perror("csrpc");
      return -1;
  }

  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(addr));

  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);
  unlink(path);

  if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
      perror("csrpc");
      close(fd);
      return -1;
  }

  if (listen(fd, 5) < 0) {
      perror("csrpc");
      close(fd);
      return -1;
  }

  int flags = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);

  return fd;
}

static bool parse_call(char *buf, struct csrpc_call *call) {
  // TODO: BUFFER OVERFLOW ATTACK
  // thing is though, if they are using csrpc, they already have code
  // execution in a script, and have access to the main process via
  // the api, so i don't know if i should even worry about that for
  // now - food for thought...

  char *s;
  call->argc = strtoll(buf, &s, 10);
  s++; // move on past the null byte after the count

  if (call->argc < 1) return false; // empty call

  char **args = malloc(call->argc * sizeof(char*));
  if (args == NULL) return false;

  for (unsigned int i = 0; i < call->argc; i++) {
    char *arg = strdup(s);
    args[i] = arg;
    s += strlen(arg) + 1;
  }

  call->args = args;

  return true;
}

static void exec_call(struct csrpc_call *call, t_csrpc_handler handler,
               void *user_state, struct csrpc_resp *resp) {
  resp->response = strdup("no handler defined");
  resp->status = 1;

  if (handler) {
    struct csrpc_resp new_resp = handler(call, user_state);
    resp->response = strdup(new_resp.response);
    resp->status = new_resp.status;
  } else {
    printf("csrpc: %s(", call->args[0]);
    for (unsigned int i = 1; i < call->argc; i++) {
      printf("\"%s\", ", call->args[i]);
    }
    printf("\b\b) \b");
  }
}

static void free_call(struct csrpc_call *call) {
  for (unsigned int i = 0; i < call->argc; i++) {
    free(call->args[i]);
  }
  free(call->args);
}

static void write_resp(struct server *server, struct csrpc_resp *resp) {
  dprintf(server->client_fd, "%d%c%s%c%c", resp->status, '\0',
          resp->response, '\0', EOT);
}

static void try_run_single_rpc_command(struct server *server,
                                void *user_state,
                                t_csrpc_handler handler) {
  #define close_conn()        \
    close(server->client_fd); \
    server->client_fd = -1;   \
    server->len = 0;          \

  if (server->client_fd < 0) {
    int fd = accept(server->listen_fd, NULL, NULL);

    if (fd < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) return;
      perror("csrpc");
      return;
    }

    server->client_fd = fd;
  }

  char c;
  ssize_t n = read(server->client_fd, &c, 1);

  if (n == 1 && c == EOT) {
    struct csrpc_call call;
    if (parse_call(server->buf, &call)) {
      struct csrpc_resp resp;
      exec_call(&call, handler, user_state, &resp);
      free_call(&call);
      write_resp(server, &resp);
    }
  } else if (n == 1) {
    if (server->len >= BUFSIZE) {
      close_conn();
      return;
    }
    server->buf[server->len] = c;
    server->len++;
  } else if (n == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
  } else if (n == 0) {
    close_conn();
  } else {
    perror("csrpc");
    exit(0);
  }
}

void csrpc_wrap(pid_t pid, t_csrpc_handler handler,
                void *user_state) {
  struct server server = {
    .listen_fd = create_server_socket(CSRPC_PATH),
    .client_fd = -1,
    .len = 0,
  };

  int status;
  pid_t result = 0;

  while (!result) {
    result = waitpid(pid, &status, WNOHANG);

    if (result < 0) {
      perror("csrpc");
      return;
    }

    try_run_single_rpc_command(&server, user_state, handler);
  }
}

void csrpc_run(char *cmd, char *binpath, t_csrpc_handler handler,
               void *user_state) {
  pid_t pid = fork();

  if (pid == 0) {
    setenv("CSRPC_PATH", CSRPC_PATH, 1);

    char *orig_path = getenv("PATH");
    char new_path[1024];
    snprintf(new_path, sizeof(new_path), "%s:%s", binpath, orig_path);
    setenv("PATH", new_path, 1);

    execl("/bin/sh", "bash", "-c", cmd, NULL);
    perror("csrpc");
    return;
  } else if (pid <= 0) {
    perror("csrpc");
    return;
  }

  csrpc_wrap(pid, handler, user_state);
}
