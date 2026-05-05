#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "server.h"

struct csrpc_resp rpc_handler(struct csrpc_call *call, void *_) {
  unsigned int argc = call->argc;
  char **args = call->args;

  printf(":");
  for (unsigned int i = 0; i < argc; i++) {
    printf("%s ", args[i]);
  }
  printf("\n");
  
  struct csrpc_resp resp = {
    .response = "success\n",
    .status = 0,
  };
  return resp;
}

int main() {
  csrpc_run("send-cmd cmd arg1 arg2 ...",
            "./build/", rpc_handler, NULL);
  return 0;
}
