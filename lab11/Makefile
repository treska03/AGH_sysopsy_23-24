CC = gcc
BUILD_DIR = ./build
CFLAGS = -Wall -Wextra -O2

all: client server

client:
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) client.c -o $(BUILD_DIR)/client

server:
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) server.c -o $(BUILD_DIR)/server
	
clean:
	rm -rf $(BUILD_DIR)