CC = gcc
BUILD_DIR = ./build
CFLAGS = -Wall -Wextra -O2

all: signals
	make signals

signals: signals.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) signals.c -o $(BUILD_DIR)/signals

clean:
	rm -rf $(BUILD_DIR)