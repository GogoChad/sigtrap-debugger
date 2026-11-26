CC = gcc
CPPFLAGS += -Iinclude
CFLAGS += -std=c11 -Wall -Wextra -Wpedantic -g

SOURCES = src/main.c src/process.c

sigtrap: $(SOURCES) include/process.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $(SOURCES) $(LDLIBS) -o $@
