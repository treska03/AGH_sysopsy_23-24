#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

double g_start;
double g_end;

double f(double x) {
    return 4/(x*x + 1);
}

double integrate(double start, double end) {
    int n = 100000;
    double recWidth = (end - start) / n;
    double result = 0;

    for(int i=0; i<n; i++) {
        result += f(start + recWidth*i);
    }

    return result/n;
}


int main() {
    int inPipe = open("datapipe", O_RDONLY);

    double res;
    int linesRetrieved = 0;

    while(read(inPipe, &res, sizeof(double)) > 0) {
        if(linesRetrieved == 0) {
            g_start = res;
            linesRetrieved++;
        }
        else if (linesRetrieved == 1) {
            g_end = res;
            linesRetrieved++;
        }
    }
    close(inPipe);
    
    int outPipe = open("resultpipe", O_WRONLY);
    double calcResult = integrate(g_start, g_end);
    write(outPipe, &calcResult, sizeof(double));

    close(outPipe);

    return EXIT_SUCCESS;
}