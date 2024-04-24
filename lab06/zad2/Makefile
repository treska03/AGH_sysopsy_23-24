CC = gcc
BUILD_DIR = ./build
CFLAGS = -Wall -Wextra -O2

all: stdin_calc stdout_calc

stdin_calc:
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) stdin_calc.c -o $(BUILD_DIR)/stdin_calc

stdout_calc:
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) stdout_calc.c -o $(BUILD_DIR)/stdout_calc
	
clean:
	rm -rf $(BUILD_DIR)