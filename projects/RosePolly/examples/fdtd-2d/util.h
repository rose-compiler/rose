#include <unistd.h>
#include <sys/time.h>

#ifdef TIME
#define IF_TIME(foo) foo;
#else
#define IF_TIME(foo)
#endif

double rtclock()
{
    struct timezone Tzp;
    struct timeval Tp;
    int stat;
    stat = gettimeofday (&Tp, &Tzp);
    if (stat != 0) printf("Error return from gettimeofday: %d",stat);
    return(Tp.tv_sec + Tp.tv_usec*1.0e-6);
}

void init_array()
{
    int i, j;

    for (i=0; i<nx+1; i++)  {
        for (j=0; j<ny; j++)  {
            ey[i][j] = 0;
        }
    }

    for (i=0; i<nx; i++)  {
        for (j=0; j<ny+1; j++)  {
            ex[i][j] = 0;
        }
    }

    for (j=0; j<ny; j++)  {
        ey[0][j] = ((double)j)/ny;
    }

    for (i=0; i<nx; i++)    {
        for (j=0; j<ny; j++)  {
            hz[i][j] = 0;
        }
    }
}


void print_array()
{
    int i, j;

    for (i=0; i<nx; i++) {
        for (j=0; j<ny; j++) {
            fprintf(stderr, "%lf ", hz[i][j]);
            if (j%80 == 20) fprintf(stderr, "\n");
        }
    }
    fprintf(stderr, "\n");
}
