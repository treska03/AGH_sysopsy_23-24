#include <dlfcn.h>
#include <stdio.h>


int main(void) {
    //
    void *handler = dlopen("./libCollatzLibrary.so", RTLD_LAZY);
    if (!handler) { printf("Error while opening library \n"); return 0; }

    int (*collatz)(int input, int maxIter);
    collatz = dlsym(handler, "test_collatz_convergence");
    
    if (dlerror() != 0) { printf("Function not found \n"); return 0; }
    printf( "%d \n", collatz(10, 10) );
    
    dlclose(handler);

    return 0;
}