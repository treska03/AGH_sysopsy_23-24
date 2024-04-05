#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

pid_t CATCHER_PID;
int PENDING = 1;


void responseHandler(int signal) {
    if(signal == SIGUSR1) {
        PENDING = 0;
    }
    else {
        exit(EXIT_FAILURE);
    }
}

void addListener() {
    struct sigaction action;
    sigemptyset(&action.sa_mask);
    sigaddset(&action.sa_mask, SIGUSR1);

    action.sa_handler = responseHandler;
    action.sa_flags = 0;

    sigaction(SIGUSR1, &action, NULL);
}


int sendMode(int mode) {
    // return 0 on sucess; 1 on failure
    union sigval value;
    value.sival_int = mode;
    
    addListener();
    sigqueue(CATCHER_PID, SIGUSR1, value);

    int waitCount = 50;
    while(PENDING && waitCount-- > 0) {
        sleep(0.01);
    }

    int result = PENDING;
    PENDING = 1;
    
    return result;
}

int main(int argc, char* argv[]) {

    if(argc < 3) {
        return -1;
    }

    CATCHER_PID = (pid_t) atoi(argv[1]);
    int mode = atoi(argv[2]);

    int returnCode = sendMode(mode);

    printf(!returnCode ? "Sucessfully sent signal\n" : "Ooops, something went wrong\n");
    
    exit(returnCode);
}