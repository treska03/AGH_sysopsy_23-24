#ifndef COMMON_H_
#define COMMON_H_

#define DESCRIPTOR_NAME "printers_descriptor"
#include <semaphore.h>

typedef enum {
    WAITING = 0,
    PRINTING = 1
} printer_state_t;

typedef struct {
    sem_t printer_semaphore;
    char printer_buffer[11]; // +1 for \0 terminator
    printer_state_t printer_state;
} printer_t;

typedef struct {
    printer_t printers[100];
    int number_of_printers;
} memory_map_t;

#endif