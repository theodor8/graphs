

CC = gcc
CFLAGS = -Wall -pedantic


.PHONY: clean run

all: bin/main

bin/main: obj/main.o obj/graph.o obj/list.o
	$(CC) $^ -o $@

obj/%.o: src/%.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm bin/* obj/*


run: bin/main
	./bin/main

memrun: bin/main
	valgrind ./bin/main
