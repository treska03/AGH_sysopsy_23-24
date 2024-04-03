#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>


void handleUSR1() {
    printf("SIGUSR1 has been received!\n");
}

int main(int argc, char *argv[]) {

    if(argc != 2) {
        return -1;
    }


    char* ENTRY_ARG = argv[1];

    if(strcmp(ENTRY_ARG, "ignore") == 0) {
        signal(SIGUSR1, SIG_IGN);
    }
    else if (strcmp(ENTRY_ARG, "none") == 0) {
        signal(SIGUSR1, SIG_DFL);
    }
    else if (strcmp(ENTRY_ARG, "handler") == 0) {
        signal(SIGUSR1, handleUSR1);
    }
    else if (strcmp(ENTRY_ARG, "mask") == 0) {
        sigset_t blockMask, pendingSigs;
        sigemptyset(&blockMask);
        sigaddset(&blockMask, SIGUSR1);

        sigprocmask(SIG_SETMASK, &blockMask, NULL);

        raise(SIGUSR1);

        sigpending(&pendingSigs);

        printf("Signal SIGUSR1 is%s pending!\n", !sigismember(&pendingSigs, SIGUSR1) ? " not" : "");

        return 0;
    }
    else {
        return -1;
    }

    raise(SIGUSR1);

    return 0;
}