# Makefile for aurora_c_demo (POSIX / MinGW)
CC=gcc
CFLAGS=-std=c11 -O2 -Wall -Wextra
TARGET=aurora_c_demo
SRC=aurora_c_demo.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all clean
