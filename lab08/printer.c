#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

#include "common.h"

volatile int keep_alive = 1;

void SIGNAL_handler(int signum) {
    keep_alive = 0;
}

int main(int argc, char* argv[]) {

    if(argc < 2) return EXIT_FAILURE;

    int printer_num = atoi(argv[1]);
    if(printer_num > 100) {
        printf("Too many printers, exiting\n");
        return EXIT_FAILURE;
    }

    int memory_fd = shm_open(DESCRIPTOR_NAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if(memory_fd < 0) {
        printf("Error while opening shared mem shm_open\n");
        return EXIT_FAILURE;
    }

    if(ftruncate(memory_fd, sizeof(memory_map_t)) < 0) {
        printf("Error while truncating shared mem ftruncate\n");
        return EXIT_FAILURE;
    }

    memory_map_t* memory_map = mmap(NULL, sizeof(memory_map_t), PROT_READ | PROT_WRITE, MAP_SHARED, memory_fd, 0);
    if(memory_map == MAP_FAILED) {
        printf("Error while maping shared mem mmap\n");
        return EXIT_FAILURE;
    }


    memset(memory_map, 0, sizeof(memory_map_t));
    memory_map->number_of_printers = printer_num;

    for(int sig = 1; sig < SIGRTMAX; sig++) {
        signal(sig, SIGNAL_handler);
    }

    for(int i=0; i<printer_num; i++) {
        // printer_t printer = memory_map->printers[i];

        if(sem_init(&memory_map->printers[i].printer_semaphore, 1, 1) < 0) {
            printf("Initializing semaphore failed\n");
            continue;
        }

        pid_t pid = fork();
        if(pid == -1) {
            printf("Spawning child #%d failed, skipping\n", i);
            continue;
        }

        else if(pid == 0) {
            while(keep_alive) {
                if(memory_map->printers[i].printer_state == PRINTING) {
                    for(int j=0; j<10; j++) {
                        printf("%c", memory_map->printers[i].printer_buffer[j]);
                        sleep(1);
                    }
                    
                    printf("\n");
                    fflush(stdout); // idk what it does
                    
                    memory_map->printers[i].printer_state = WAITING;
                    sem_post(&memory_map->printers[i].printer_semaphore);
                }
                sleep(0.1); // wait for print order
            }
            exit(0);
        }
    }

    while(wait(NULL) > 0);

    // destroy semaphores
    for(int i=0; i<printer_num; i++) {
        sem_destroy(&memory_map->printers[i].printer_semaphore);
    }

    munmap(memory_map, sizeof(memory_map_t));
    shm_unlink(DESCRIPTOR_NAME);

    return EXIT_SUCCESS;
}