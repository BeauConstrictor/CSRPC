#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "csrpc.h"

int main() {
  char buf[256];

  while (1) {
    printf("$ ");
    fflush(stdout);

    if (!fgets(buf, sizeof(buf), stdin))
        break;

    buf[strcspn(buf, "\n")] = '\0';

    if (strcmp(buf, "exit") == 0) return 0;

    FILE* f = csrpc_run(buf, "./init.sh", "./build/", NULL, NULL);
    int c;
    while ((c = fgetc(f)) != EOF) {
      putchar(c);
    }
    fclose(f);
  }

  return 0;
}
