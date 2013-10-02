#include "macroIdsDef.h"
#include "macroIds.h"
#include <omp.h>

#define THREADS_1 2

int tmp()
{
    return 0;
}

int main(int argc, char ** argv)
{
    int i = 0;
#pragma omp parallel num_threads( THREADS_1 )
    {
        i++;
    }
#pragma omp parallel num_threads( THREADS_2 + THREADS_3 )
    {
        i++;
    }

    return !(i==0);
}
