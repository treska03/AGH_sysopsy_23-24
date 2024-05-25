#include "grid.h"
#include <pthread.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>

const int grid_width = 30;
const int grid_height = 30;

char *create_grid()
{
    return malloc(sizeof(char) * grid_width * grid_height);
}

void destroy_grid(char *grid)
{
    free(grid);
}

void draw_grid(char *grid)
{
    for (int i = 0; i < grid_height; ++i)
    {
        // Two characters for more uniform spaces (vertical vs horizontal).
        for (int j = 0; j < grid_width; ++j)
        {
            if (grid[i * grid_width + j])
            {
                mvprintw(i, j * 2, "■");
                mvprintw(i, j * 2 + 1, " ");
            }
            else
            {
                mvprintw(i, j * 2, " ");
                mvprintw(i, j * 2 + 1, " ");
            }
        }
    }

    refresh();
}

void init_grid(char *grid)
{
    for (int i = 0; i < grid_width * grid_height; ++i)
        grid[i] = rand() % 2 == 0;
}

bool is_alive(int row, int col, char *grid)
{
    int count = 0;
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0)
            {
                continue;
            }
            int r = row + i;
            int c = col + j;
            if (r < 0 || r >= grid_height || c < 0 || c >= grid_width)
            {
                continue;
            }
            if (grid[grid_width * r + c])
            {
                count++;
            }
        }
    }

    if (grid[row * grid_width + col])
    {
        if (count == 2 || count == 3)
            return true;
        else
            return false;
    }
    else
    {
        if (count == 3)
            return true;
        else
            return false;
    }
}

void update_grid(char *src, char *dst)
{
    for (int i = 0; i < grid_height; ++i)
    {
        for (int j = 0; j < grid_width; ++j)
        {
            dst[i * grid_width + j] = is_alive(i, j, src);
        }
    }
}

// ==================================================
// ================== UNIVERSAL =====================
// ==================================================
const int grid_size = grid_width * grid_height;

typedef struct {
    char *src;
    char *dst;
    int start;
    int end;
} thread_args_t;

pthread_t *threads = NULL;
thread_args_t *args = NULL;

void free_memory() {
    free(threads);
    free(args);
}

void ignore_handler(int signum) {}

void* update_cell(void *args) {
    thread_args_t *thread_args = (thread_args_t*) args;
    int start = thread_args->start, end = thread_args->end;

    while (true) {
        for (int k = start; k <= end; k++) {
            thread_args->dst[k] = is_alive(k / grid_width, k % grid_height, thread_args->src);
        }
        char *temp = thread_args->src;
        thread_args->src = thread_args->dst;
        thread_args->dst = temp;
        
        pause();
    }

    pthread_exit(0);
}

void update_grid_concurrent(char *src, char *dst, int threads_no) {
    threads_no = threads_no > grid_size ? grid_size : threads_no;
    // Init threads.
    if (threads == NULL) {
        signal(SIGUSR1, &ignore_handler);

        threads = malloc(threads_no * sizeof(pthread_t));
        args = malloc(threads_no * sizeof(thread_args_t));

        int block_size;
        int remainder = grid_size % threads_no;
        int curr_start = 0;
        
        for (int i = 0; i < threads_no; i++) {
            block_size = grid_size / threads_no;
            if(remainder-- > 0) block_size++;

            args[i].src = src;
            args[i].dst = dst;
            args[i].start = curr_start + 1;
            args[i].end = curr_start + block_size;

            curr_start = args[i].end;

            pthread_create(&threads[i], NULL, update_cell, (void*) &args[i]);
        }
    }

    for (int i = 0; i < threads_no; i++)
        pthread_kill(threads[i], SIGUSR1);
}
