#include <stdio.h>


int test_collatz_convergence(int input, int maxIter);

int main() {
    printf("%d \n", test_collatz_convergence(10, 10));
    return 0;
}