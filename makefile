CC ?= gcc
CCFLAGS ?= -Wall -Wextra

all: build/libcsrpc.a build/send-cmd

build/:
	mkdir -p build/

build/server.o: src/server.c include/csrpc.h build/
	$(CC) $(CCFLAGS) -c $< -o $@ -I./include/

build/send-cmd: src/client.c build/
	$(CC) $(CCFLAGS) $< -o $@

build/libcsrpc.a: build/server.o
	ar rcs $@ $^

build/example: src/example.c build/libcsrpc.a include/csrpc.h build/ 
	$(CC) $(CCFLAGS) $< -L./build/ -lcsrpc -o $@ -I./include/

.PHONY: all run clean

clean:
	rm build/*

run: build/example
	build/example
