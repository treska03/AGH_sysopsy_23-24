#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int arc, char *argv[]) {

    if(arc != 2) {
        printf("Please provide number of processes!\n");
        return -1;
    }

    int processAmount = atoi(argv[1]);
    printf("PROCES AMOUNT : %d\n", processAmount);

    for(int i=0; i<processAmount; i++) {
        if(fork() == 0) {
            printf("Identyfikator procesu macierzystego to: --- %d ---, a własny to --- %d ---\n", getppid(), getpid());
            exit(0);
        }
    }

    int status;
    while (wait(&status) > 0);    
    // waitpid(0, NULL, 0); // ale czemu tak nie może być?
    // według skryptu to powinno czekać na wszystkie dzieci

    printf("Liczba zespawnowanych procesów: --- %d ---\n", processAmount);
    return 0;
}