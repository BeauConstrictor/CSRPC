all: build/libcsrpc.a build/send-cmd

build/:
	mkdir -p build/

build/server.o: src/server.c
	gcc -Wall -Wextra -c $< -o $@

build/send-cmd: src/client.c
	gcc -Wall -Wextra $< -o $@

build/libcsrpc.a: build/server.o
	ar rcs $@ $^

build/test: src/test.c build/libcsrpc.a build/send-cmd
	gcc $< -L./build/ -lcsrpc -o $@

.PHONY: all run clean

clean:
	rm build/*

run: build/test
	build/test
