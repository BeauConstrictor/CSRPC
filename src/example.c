#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "csrpc.h"

const char *helptext = ""
"CSRPC Example Program\n"
"\n"
"This is effectively an sh repl, but with the 'sc' command available.\n"
"sc will send a CSRPC command to the example program, which it will\n"
"handle simply by outputting the command it recieved.\n";

int main() {
  char buf[256];

  printf("%s", helptext);

  while (1) {
    printf("$ ");
    fflush(stdout);

    if (!fgets(buf, sizeof(buf), stdin))
        break;

    buf[strcspn(buf, "\n")] = '\0';

    if (strcmp(buf, "exit") == 0) return 0;

    FILE* f = csrpc_run(buf, "./init.sh", NULL, NULL);
    int c;
    while ((c = fgetc(f)) != EOF) {
      putchar(c);
    }
    fclose(f);
  }

  return 0;
}
