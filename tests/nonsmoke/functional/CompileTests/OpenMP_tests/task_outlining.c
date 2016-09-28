#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

int N[20][20];
int v1[5];
int j = 0;
int * j_ptr = &j;

void outlining( int M[10][10], int v2[5], int (*v4) [5] )
{
#pragma omp task shared( N )
    N[0][0] = 1;
#pragma omp taskwait
#pragma omp task firstprivate( N )
    N[0][0] = 2;
#pragma omp taskwait
    
#pragma omp task shared( M )
    M[0][0] = 3;
#pragma omp taskwait
#pragma omp task firstprivate( M )
    M[0][0] = 4;
#pragma omp taskwait

    int diag[10][10];
#pragma omp task shared( diag )
    diag[0][0] = 5;
#pragma omp taskwait
#pragma omp task firstprivate( diag )
    diag[0][0] = 6;
#pragma omp taskwait

    int (*c1)[10] = calloc(sizeof(int), 10 * 10);
#pragma omp task shared( c1 )
    c1[0][0] = 7;
#pragma omp taskwait
#pragma omp task firstprivate( c1 )
    c1[0][0] = 8;
#pragma omp taskwait

#pragma omp task shared( v1 )
    v1[0] = 9;
#pragma omp taskwait
#pragma omp task firstprivate( v1 )
    v1[0] = 10;
#pragma omp taskwait
    
#pragma omp task shared( v2 )
    v2[0] = 11;
#pragma omp taskwait
#pragma omp task firstprivate( v2 )
    v2[0] = 12;
#pragma omp taskwait

    float * v3 = (float *) malloc(sizeof(float) * 10);
#pragma omp task shared( v3 )
    v3[0] = 13;
#pragma omp taskwait
#pragma omp task firstprivate( v3 )
    v3[0] = 14;
#pragma omp taskwait
    
    int i;
#pragma omp task shared( i )
    i = 15;
#pragma omp taskwait
#pragma omp task firstprivate( i )
    i = 16;
#pragma omp taskwait
    
    int k;
    int * i_ptr = &k;
#pragma omp task shared( i_ptr )
    *i_ptr = 17;
#pragma omp taskwait
#pragma omp task firstprivate( i_ptr )
    *i_ptr = 18;    
#pragma omp taskwait
#pragma omp task shared( j_ptr )
    *j_ptr = 19;
#pragma omp taskwait
#pragma omp task firstprivate( j_ptr )
    *j_ptr = 20;
#pragma omp taskwait

#pragma omp task shared( v4 )
    v4[0][0] = 21;
#pragma omp taskwait
#pragma omp task firstprivate( v4 )
    v4[0][0] = 22;
#pragma omp taskwait

    assert( N[0][0] == 1 );
    assert( M[0][0] == 4 );
    assert( diag[0][0] == 5 );
    assert( c1[0][0] == 8 );
    assert( v1[0] == 9 );
    assert( v2[0] == 12 );
    assert( v3[0] == 14 );
    assert( i == 15 );
    assert( *i_ptr == 18 );
    assert( *j_ptr == 20 );
    assert( v4[0][0] == 22 );
}

int main( int argc, char** argv )
{
    int M[10][10];
    int v2[5];
    int (*v4)[5] = calloc(sizeof(int), 5 * 5);
    
    outlining( M, v2, v4 );
    
    return 0;
}
