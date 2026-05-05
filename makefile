all: build/libcsrpc.a build/send-cmd

build/:
	mkdir -p build/

build/server.o: build/ src/server.c
	gcc -Wall -Wextra -c $< -o $@

build/send-cmd: build/ src/client.c
	gcc -Wall -Wextra $< -o $@

build/libcsrpc.a: build/ build/server.o
	ar rcs $@ $^

build/test: build/ src/test.c build/libcsrpc.a
	gcc $< -L./build/ -lcsrpc -o $@

.PHONY: all run clean

clean:
	rm build/*

run: build/test
	build/test
