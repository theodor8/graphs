CC = gcc
CFLAGS = -Wall -pedantic -I/opt/homebrew/include
LIBS = -L/opt/homebrew/lib -lSDL2

SDIR = src
ODIR = obj

SRCS := $(wildcard $(SDIR)/*.c)
HDRS := $(wildcard $(SDIR)/*.h)
OBJS := $(patsubst $(SDIR)/%.c,$(ODIR)/%.o,$(SRCS))

$(ODIR)/%.o: $(SDIR)/%.c $(HDRS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/main: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

run: $(ODIR)/main
	./$^

memrun: $(ODIR)/main
	leaks -atExit -- ./$^

clean:
	rm -f $(ODIR)/*

.PHONY: clean run
