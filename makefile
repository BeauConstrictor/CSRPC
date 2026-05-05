all: build/test build/server.o build/client

build/:
	mkdir -p build/

build/test: build/test.o build/server.o
	gcc build/test.o build/server.o -o build/test

build/server.o: src/server.c
	gcc -Wall -Wextra -c src/server.c -o build/server.o

build/test.o: src/test.c src/server.c build/client
	gcc -Wall -Wextra -c src/test.c -o build/test.o

build/client: src/client.c
	gcc -Wall -Wextra src/client.c -o build/client

.PHONY: all run

run: build/test
	build/test
