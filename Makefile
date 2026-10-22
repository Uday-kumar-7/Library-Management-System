# Makefile for Library Management System
CC ?= gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude -O2
SRCS = src/utils.c src/book.c src/bst.c src/stack.c src/queue.c src/issue.c src/file_io.c src/main.c
OBJS = $(SRCS:.c=.o)
TARGET = library_system.exe

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /Q src\*.o $(TARGET) 2>nul || rm -f src/*.o $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
