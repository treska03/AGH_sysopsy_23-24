#include <stdio.h>
#include "../lib/integrate.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>


int main(int argc, char* argv[]) {
    if(argc < 3) return -1;
    
    time_t start,end;
    double dif;

    time (&start);

    double recWidth = atof(argv[1]);
    int noProcesses = atoi(argv[2]);
    double processLen = 1.0 / noProcesses;

    int currProcess = 0;
    // Create 2d array for pipes  
    int fd[noProcesses][2];

    do {
        if(pipe(fd[currProcess]) == -1) exit(EXIT_FAILURE);
        pid_t pid = fork();
        if(pid == -1) {
            exit(EXIT_FAILURE);
        } 
        else if (pid == 0) {
            close(fd[currProcess][0]);
            
            double res = integrate(recWidth, processLen*currProcess, processLen*(currProcess+1))/noProcesses;
            write(fd[currProcess][1], &res, sizeof(res));
            close(fd[currProcess][1]);
            exit(0);
        }
        else {
            close(fd[currProcess][1]);
        }

        currProcess++;
    } while ( currProcess < noProcesses );

    while(wait(NULL) > 0);
    
    double integrationTotal = 0.0;
    for(int i=0; i< noProcesses; i++) {
        double res;
        read(fd[i][0], &res, sizeof(double));
        integrationTotal+=res;
        close(fd[i][0]);
    }

    printf("Provided arguments:\n -Rectangle width = %f\n -Number of processes = %d\n", recWidth, noProcesses);
    printf("Result = %f\n", integrationTotal);

    time (&end);
    dif = difftime (end,start);
    printf("Time elapsed = %fs\n", dif);

    return EXIT_SUCCESS;
}