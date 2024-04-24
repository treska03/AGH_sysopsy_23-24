CC = gcc
BUILD_DIR = ./build
CFLAGS = -Wall -Wextra -O2

all: calc

calc: integrate
	$(CC) $(CFLAGS) calc.c $(BUILD_DIR)/integrate.o -o $(BUILD_DIR)/calc

integrate:
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)/integrate.o ../lib/integrate.c
	ar rcs -o $(BUILD_DIR)/libintegrate.a $(BUILD_DIR)/integrate.o
	
clean:
	rm -rf $(BUILD_DIR)