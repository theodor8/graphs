

CC = gcc
CFLAGS = -Wall -pedantic -I/opt/homebrew/include 
LDFLAGS = -L/opt/homebrew/lib -lraylib


.PHONY: clean run

all: bin/main

bin/main: obj/main.o obj/graph.o obj/list.o obj/raygui/raygui.o
	$(CC) $(LDFLAGS) $^ -o $@

obj/%.o: src/%.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm bin/* obj/*


run: bin/main
	./bin/main

memrun: bin/main
	leaks -atExit -- ./bin/main
