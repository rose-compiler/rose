#include <assert.h>
#include <pthread.h>
#include <stdio.h>

#define NTESTS  100
#define NWORKERS 10

static size_t futex1 = 0;

static void *
worker(void *unused)
{
    size_t i;
    for (i=0; i<NTESTS; ++i)
        syscall(32100, &futex1, NWORKERS);
}

int main()
{
    pthread_t workers[NWORKERS];
    size_t i, j;

        for (i=0; i<NWORKERS; ++i)
            pthread_create(workers+i, NULL, worker, NULL);

        int total = 0;
        for (i=0; i<NWORKERS; ++i)
            pthread_join(workers[i], NULL);

    return 0;
}
