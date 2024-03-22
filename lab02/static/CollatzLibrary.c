
int collatz_conjecture(int input) {
    return input % 2 == 0 ? input/2 : 3*input+1;
}

int test_collatz_convergence(int input, int max_iter) {
    if(input == 1) return 0;

    int curr_iter = 0;
    
    while(curr_iter < max_iter) {
        curr_iter++;
        input = collatz_conjecture(input);
        if(input == 1) return curr_iter;
    }
    
    return -1;
}