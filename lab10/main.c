#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

volatile int working_reindeers_no = 0;

pthread_mutex_t santa_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reindeers_mutexes[9] = {
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER
};
pthread_mutex_t working_reindeers_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t santa_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t reindeer_cond = PTHREAD_COND_INITIALIZER;


void* reindeer_loop(void *args) {
    int id = *(int*) args;
    srand(id);

    for(int cycle = 0; cycle < 4; cycle++) {
        sleep(rand() % 6 + 5);

        pthread_mutex_lock(&working_reindeers_mutex);

        // increment counter and check if all reindeers are back
        working_reindeers_no++;
        
        printf("Renifer #%d: czeka %d reniferów\n", id, working_reindeers_no);

        if (working_reindeers_no == 9) {
            printf("Renifer #%d: budzę Mikołaja\n", id);
            pthread_cond_broadcast(&santa_cond);
        }
        pthread_mutex_unlock(&working_reindeers_mutex);

        pthread_mutex_lock(&reindeers_mutexes[id]);
        pthread_cond_wait(&reindeer_cond, &reindeers_mutexes[id]);
        pthread_mutex_unlock(&reindeers_mutexes[id]);
    }
    pthread_exit(0);
}

int main() {
    pthread_t reindeers_threads[9];
    for(int i=0; i<9; i++) {
        // create threads
        pthread_create(&reindeers_threads[i], NULL, reindeer_loop, (void*) &i);
    }

    srand(10);
    int cycle = 0;
    while(cycle++ < 4) {
        pthread_mutex_lock(&santa_mutex);

        pthread_cond_wait(&santa_cond, &santa_mutex);

        printf("Mikołaj: dostarczam zabawki\n");
        sleep(rand() % 3 + 2);
        printf("Mikołaj: zasypiam\n");
        
        working_reindeers_no = 0;
        pthread_cond_broadcast(&reindeer_cond);

        pthread_mutex_unlock(&santa_mutex); 
    }

    for(int i=0; i<9; i++) {
        pthread_join(reindeers_threads[i], NULL);
    }
    return EXIT_SUCCESS;
}