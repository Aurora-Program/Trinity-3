# Makefile for aurora demos (POSIX / MinGW)
CC=gcc
CFLAGS=-std=c11 -O2 -Wall -Wextra

# existing minimal demo (may be experimental)
TARGET1=aurora_c_demo
SRC1=aurora_c_demo.c

# new Cref streaming demo (default build)
TARGET2=phi_cref_demo
SRC2=phi_cref_demo.c

TARGET3=interactive_cli
SRC3=interactive_cli.c bootstrap_loader.c
TARGET4=aurora_showcase
SRC4=aurora_showcase.c

all: $(TARGET2) $(TARGET3) $(TARGET4)

$(TARGET1): $(SRC1)
	$(CC) $(CFLAGS) -o $(TARGET1) $(SRC1)

$(TARGET2): $(SRC2)
	$(CC) $(CFLAGS) -o $(TARGET2) $(SRC2)

$(TARGET3): $(SRC3)
	$(CC) $(CFLAGS) -o $(TARGET3) $(SRC3)

$(TARGET4): $(SRC4)
	$(CC) $(CFLAGS) -o $(TARGET4) $(SRC4)

clean:
	rm -f $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4)

.PHONY: all clean
