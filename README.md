# CSRPC (C/Shell Remote Procedure Calling)

`csrpc` is a tiny library for adding scriptable configuration and
commands (like `vim`'s `ex`-commands) to your program. Instead of
bringing a big dependency on a scripting language like Lua, or rolling
your own DSL, `csrpc` outsources the work to the POSIX `sh`, so users
can make use of it's vast ecosystem of utilities in their configuration,
while also not needing to learn an new language - they can adapt
`csrpc` to work with any scripting language if they want.

## Getting Started

To get started with CSRPC, add it as a submodule in your project and
`make` it:

```
$ git submodule add "https://github.com/beauconstrictor/csrpc.git" lib/csrpc/
$ cd lib/csrpc/
$ make
```

This will create a `libcsrpc.a`, which you can use to call shell scripts,
wrapping them with `csrpc` (you can find `csrpc.h` in `src/`).

Here is a simple example:

```c
#include <string.h>
#include <stdio.h>

#include "csrpc.h"

int main() {
  char buf[256];

  while (1) {
    printf("$ ");
    fflush(stdout);

    if (!fgets(buf, sizeof(buf), stdin))
        break;

    buf[strcspn(buf, "\n")] = 0;

    csrpc_run(buf, "csrpc-lib", NULL, NULL);
  }
}
```

This will automatically echo CSRPC commands that the script calls.

Here, `csrpc-lib` is a dir that will be added to `sh`'s PATH, which
allows you to add a custom 'stdlib' of sorts full of shell scripts
that make it easy to interface with your program. The most basic
command that should go in here is `send-cmd`, which takes a command
and some arguments and calls them, printing the command's output
and mimicking it's exit code.

To learn how to handle CSRPC commands, see `csrpc.h`, as this example
just uses CSRPC's automatic echoing when NULL is supplied as the
command handler.

## License

This library is licensed under the open-source MIT license. See the
[LICENSE.txt](/LICENSE.txt).
