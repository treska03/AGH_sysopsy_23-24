#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>


int main(int argc, char* argv[]) {

    if(argc != 3) {
        printf("Provide start and finish of integral interval\n");
        exit(EXIT_FAILURE);
    }
    
    if(mkfifo("datapipe", S_IRWXU) == -1 && errno != EEXIST) {
        printf("Creating of datapipe failed!\n");
        exit(EXIT_FAILURE);
    }
    if(mkfifo("resultpipe", S_IRWXU) == -1 && errno != EEXIST) {
        printf("Creating of resultpipe failed!\n");
        exit(EXIT_FAILURE);
    }
    
    int outPipe = open("datapipe", O_WRONLY);

    double range_start = atof(argv[1]);
    double range_end = atof(argv[2]);
    
    write(outPipe, &range_start, sizeof(double));
    write(outPipe, &range_end, sizeof(double));

    close(outPipe);

    int inPipe = open("resultpipe", O_RDONLY);

    char res[sizeof(double)];
    while(read(inPipe, &res, sizeof(double)) > 0) {
        printf("Integral is equal to %s\n", res);
    }

    close(inPipe);


    return EXIT_SUCCESS;
}