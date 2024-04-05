#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int g_changeCount = 0;
int g_mode = -1;

void sendResponse(pid_t ppid) {
    union sigval val;
    sigqueue(ppid, SIGUSR1, val);
}

void handler(int signo, siginfo_t *siginfo, void *sth) {
    if(signo != SIGUSR1) {
        raise(SIGTERM);
    }
    else {
        g_mode = siginfo->si_value.sival_int;
        g_changeCount++;
        sendResponse(siginfo->si_pid);
    }
}

void listen(int signal) {
    struct sigaction sigact;
    sigemptyset(&sigact.sa_mask);
    sigaddset(&sigact.sa_mask, signal);

    sigact.sa_sigaction = handler;
    sigact.sa_flags = SA_SIGINFO; // needed
    sigaction(SIGUSR1, &sigact, NULL);
}



int main () {

    printf("Catcher's PID: %d, awaiting signals\n", getpid());

    listen(SIGUSR1);

    while(1) {
        switch (g_mode)
        {
        case 0:
            for(int i=0; i<=100; i++){
                printf("%d\n", i);
            }
            g_mode=-1;
            break;
        case 1:
            printf("Mode changes so far: %d\n", g_changeCount);
            g_mode=-1;
            break;
        case 2:
            printf("Exitting catcher!\n");
            exit(0);
            break;     
        default:
            sleep(0.01);
            break;
        }
    }

}