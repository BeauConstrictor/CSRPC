#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define EOT '\x04'

int main(int argc, char *argv[]) {
  char *path = getenv("CSRPC_PATH");
  if (!path) {
    fprintf(stderr, "csrpc: CSRPC_PATH is not set\n");
    return 1;
  }
  int fd = socket(AF_UNIX, SOCK_STREAM, 0);

  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", path);

  if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    perror("csrpc");
    return 1;
  }

  dprintf(fd, "%d%c", argc-1, '\0');
  for (int i = 1; i < argc; i++) {
    dprintf(fd, "%s%c", argv[i], '\0');
  }
  dprintf(fd, "%c", EOT);

  char buf[1024];
  size_t len = 0;
  char c = '\0';
  while (c != EOT && len < sizeof(buf)) {
    ssize_t n = read(fd, &c, 1);
    if (n > 0) {
      buf[len] = c;
      len++;
    } else if (n < 0) {
      perror("csrpc");
      return 1;
    }
  }
  buf[sizeof(buf)-1] = '\0';

  char *s;
  int status = strtoll(buf, &s, 10);
  s++;
  printf("%s", s);

  close(fd);
  return status;
}
