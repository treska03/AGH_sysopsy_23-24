CC = gcc
BUILD_DIR = ./build
CFLAGS = -Wall

all: main

main: main.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) main.c -o $(BUILD_DIR)/main

clean:
	rm -rf $(BUILD_DIR)
