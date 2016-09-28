// A test case based on IS/is.c of npb3.2-omp 
// to test the handling of #if #endif during OpenMP translation
//
// 6/9/2010, Liao
//
#include <stdio.h>


#define NUM_KEYS 1000
int key_array[NUM_KEYS], key_buff_ptr_global[NUM_KEYS]; 

void full_verify()
{
    int i, j;
    int k;
    int passed_verification =0;


/*  Now, finally, sort the keys:  */

#ifdef SERIAL_SORT
/*  Copy keys into work array; keys in key_array will be reassigned. */
#ifdef _OPENMP
#pragma omp parallel for private(i)
#endif
    for( i=0; i<NUM_KEYS; i++ )
        key_buff2[i] = key_array[i];


    /* This is actual sorting */
    for( i=0; i<NUM_KEYS; i++ )
        key_array[--key_buff_ptr_global[key_buff2[i]]] = key_buff2[i];

#else /*SERIAL_SORT*/

    /* Memory sorting can be done directly */
#ifdef _OPENMP
#pragma omp parallel for private(i,k)
#endif
    for( k=0; k<NUM_KEYS; k++ ) {
        i = (k==0)? 0 : key_buff_ptr_global[k-1];
        while ( i<key_buff_ptr_global[k] )
           key_array[i++] = k;
    }
#endif /*SERIAL_SORT*/

/*  Confirm keys correctly sorted: count incorrectly sorted keys, if any */

    j = 0;
#ifdef _OPENMP
#pragma omp parallel for private(i) reduction(+:j)
#endif
    for( i=1; i<NUM_KEYS; i++ )
        if( key_array[i-1] > key_array[i] )
            j++;


    if( j != 0 )
        printf( "Full_verify: number of keys out of sort: %d\n", j );
    else
        passed_verification++;

}

// This function is required to reproduce a bug
void rank ()
{

#ifdef _OPENMP
#pragma omp parallel 
#endif
  {
    printf("nothing here");

  }
}

