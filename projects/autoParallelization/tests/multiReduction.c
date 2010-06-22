// A feature request from an anonymous user named 'Nick'
// 5/28/2010
#include <stdlib.h>

#define num_steps 2000000000

int main(int argc, char** argv) {
    double pi = 0;
    int i;
    for (i = 0; i < num_steps; i++) {
        pi += 1.0 / (i * 4.0 + 1.0);
        pi -= 1.0 / (i * 4.0 + 3.0);
    }
    pi = pi * 4.0;
    printf("pi done - %f \n", pi);
    return (EXIT_SUCCESS);
}

