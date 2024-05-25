CC = gcc
BUILD_DIR = ./build
CFLAGS = -Wall -Wextra -O2

all: printer user

printer:
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) printer.c -o $(BUILD_DIR)/printer

user:
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) user.c -o $(BUILD_DIR)/user
	
clean:
	rm -rf $(BUILD_DIR)