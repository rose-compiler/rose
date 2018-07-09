#include <upc.h> /* Required for UPC extensions */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

shared int all_hits[THREADS];

int trial_in_disk() {
    float x = (float)((float)rand() / (float)RAND_MAX);
    float y = (float)((float)rand() / (float)RAND_MAX);
    if((pow(x, 2) + pow(y, 2)) < 1) {
        return 1;
    }
    return 0;
}

int main() {
    int i, hits = 0, tot = 0, trials = 1000000;
    srand(1+MYTHREAD*17);
    for(i = 0; i < trials; ++i) {
        hits += trial_in_disk();
    }
    all_hits[MYTHREAD] += hits;
    upc_barrier;
    if(MYTHREAD == 0) {
        for(i = 0; i < THREADS; ++i) {
            tot += all_hits[i];
        }
        printf("Pi approx %g\n", 4.0*tot/(trials*THREADS));
    }
    return 1;
}
