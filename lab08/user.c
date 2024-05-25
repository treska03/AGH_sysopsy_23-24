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

void get_random_string(char* buff, int len) {
    for(int i=0; i<len; i++) {
        buff[i] = 'a' + (rand() % 26);
    }
    buff[len] = '\0';
}


int main(int argc, char* argv[]) {

    if(argc < 2) return EXIT_FAILURE;

    int user_num = atoi(argv[1]);
    if(user_num > 100) {
        printf("Too many users, exiting\n");
        return EXIT_FAILURE;
    }

    int memory_fd = shm_open(DESCRIPTOR_NAME, O_RDWR, S_IRUSR | S_IWUSR);
    if(memory_fd < 0) {
        printf("Error while opening shared mem shm_open\n");
        return EXIT_FAILURE;
    }
    memory_map_t* memory_map = mmap(
        NULL,
        sizeof(memory_map_t),
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        memory_fd,
        0
    );
    if(memory_map == MAP_FAILED) {
        printf("Error while maping shared mem mmap\n");
        return EXIT_FAILURE;
    }

    for(int i=1; i<SIGRTMAX; i++) {
        signal(i, SIGNAL_handler);
    }

    char buff[11] = {0};
    for(int process_idx=0; process_idx<user_num; process_idx++) {

        pid_t pid = fork();
        if(pid == -1) {
            printf("Spawning child #%d failed, skipping\n", process_idx);
            continue;
        }

        if(pid == 0) {
            srand(getpid());
            while(keep_alive) {

                get_random_string(buff, 10);

                int printer_idx = -1;
                for(int j=0; j<memory_map->number_of_printers; j++) {
                    
                    int val;
                    if(sem_getvalue(&memory_map->printers[j].printer_semaphore, &val) < 0){
                        printf("Error fetching sem_value\n");
                    }
                    if(val > 0) {
                        printer_idx = j;
                        break;
                    }
                }
                if(printer_idx < 0) {
                    printf("All printers are busy, randomizing\n");
                    printer_idx = rand() % memory_map->number_of_printers;
                }

                if(sem_wait(&memory_map->printers[printer_idx].printer_semaphore) < 0) {
                    printf("Error while decrementing printer #%d semaphore", printer_idx);
                }

                memcpy(&memory_map->printers[printer_idx].printer_buffer, &buff, 11);
                memory_map->printers[printer_idx].printer_state = PRINTING;

                printf("User #%d is printing on printer #%d\n", process_idx, printer_idx);
                fflush(stdout);
                sleep((rand() % 6) + 5); // sleep for 5-10 seconds
            }
            exit(0);
        }
    }

    while(wait(NULL) > 0);

    munmap(memory_map, sizeof(memory_map_t));

    return EXIT_SUCCESS;
}