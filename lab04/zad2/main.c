#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int global = 0;

int main(int argc, char* argv[]) {

    printf("My name is: %s\n", argv[0]);

    int local = 0;
    int status;
    pid_t childpid;

    if((childpid = fork()) == 0) {
        printf("child process\n");
        global++;
        local++;
        printf("child pid = %d, parent pid = %d\n", getpid(), getppid());
        printf("child's local = %d, child's global = %d\n", local, global);
        execl("/bin/ls", "-ls", argv[1], NULL);
        exit(status);
    }

    waitpid(childpid, &status, 0);
    
    printf("parent process\n");
    printf("parent pid = %d, child pid = %d\n", getpid(), childpid);
    printf("Child exit code: %d\n", status);
    printf("Parent's local = %d, parent's global = %d\n", local, global);
    
    return status;
}