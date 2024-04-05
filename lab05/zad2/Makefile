CC = gcc
BUILD_DIR = ./build
CFLAGS = -Wall -Wextra -O2

all: catcher sender

catcher: catcher.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) catcher.c -o $(BUILD_DIR)/catcher

sender: sender.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) sender.c -o $(BUILD_DIR)/sender

clean:
	rm -rf $(BUILD_DIR)