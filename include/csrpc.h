/*
 * csrpc.h - a tiny RPC library meant as an alternative to adding
 *           a large scripting language dependency to your project.
 *
 * csrpc allows shortlived external programs to act as scripts,
 * controlling your program at runtime through a series of commands.
 * these commands can be cleanly integrated into the language, so that
 * the whole system feels native, just like how Neovim uses Lua.
 *
 * csrpc stands for C/Shell Remote Procedure Calling, as shell scripts
 * are the intended language, although any scripting language would be
 * perfectly useable.
 *
 */

#ifndef CSRPC_H
#define CSRPC_H

#include <sys/types.h>

// path of csrpc's socket; hardcoded in the library.
extern const char *CSRPC_PATH;

// usually, args[0] will be used as the name of the command to run,
// and this will be parsed in the handler by a command dispatcher.
struct csrpc_call {
  unsigned int argc;
  char **args;
};

// the response to an rpc call.
// interpreted by client as a message for stdout and an exit code.
struct csrpc_resp {
  char *response;
   int status;
};

typedef struct csrpc_resp (*t_csrpc_handler)(
    struct csrpc_call *call, void *user_state);

// listen for rpc calls while the process runs, calling handler upon
// a call. user_state is passed directly to the handler.
void csrpc_wrap(pid_t pid, t_csrpc_handler handler, void *user_state);

// run a shell command, with binpath added to the start of it's PATH.
// everything else is the same as with csrpc_wrap().
void csrpc_run(char *cmd, char *binpath, t_csrpc_handler handler,
               void *user_state);

#endif // CSRPC_H
