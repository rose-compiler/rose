/*  
 * A thin runtime layer supporting MPI code generation
 * 
 *  Liao 9/17/2015
 *  */
#ifndef LIBXOMP_MPI_H
#define LIBXOMP_MPI_H

#ifdef __cplusplus
extern "C" {
#endif

/* headers */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h> // for abort()
#include <sys/time.h>
#include <string.h> // for memcpy()

/* prototype declarations */

/*Initialize MPI and obtain default rank and number of processes, return 0 if successful */
extern int xomp_init_mpi (int* argc, char** argv[], int * rank, int * nproc); 


/*Divide up a range of data into equal chunks, based on thread/process count and ID , return start and end offsets */
extern void xomp_static_even_divide_lower_upper (int start, int end, int thread_count, int thread_id, int* n_lower, int* n_upper);

/* Similar function, but use lower:size */
extern void xomp_static_even_divide_start_size (int start, int size, int thread_count, int thread_id, int* n_lower, int* n_size);

/* using a specified dimension, divide a up-to-3-D source buffer (sourceDataPointer) to 
      multiple smaller buffers in all processes,  
 store the smaller buffer of the current MPI process into distsrc
 Distributed_dimension_id starts from 0 to 2.

 The source buffer's X, Y, Z sizes already include the optional halo region. 
 Halo_size: we may pad the data partitions with boundary elements .

Element type ID: 
ID  MPI Datatype    C Type
  ----------------------------------------------
0  MPI_CHAR        signed char
1  MPI_SHORT       signed short i
2  MPI_INT         signed int
3  MPI_LONG        signed long int
4  MPI_UNSIGNED_CHAR       unsigned char
5  MPI_UNSIGNED_SHORT      unsigned short int
6  MPI_UNSIGNED    unsigned int
7  MPI_UNSIGNED_LONG       unsigned long int
8  MPI_FLOAT       float
9  MPI_DOUBLE      double
10  MPI_LONG_DOUBLE         long double
11  MPI_BYTE        (none)
12  MPI_PACKED      (none) 

TODO: support float type also
*/
extern void xomp_divide_scatter_array_to_all (double* sourceDataPointer, int x_dim_size, int y_dim_size, int z_dim_size,
 int distributed_dimension_id, int halo_size, int rank_id, int process_count, double** distsrc);

//  scattered array portions indicated by *distdest
//  need to be collected to destnationDataPointer
extern void xomp_collect_scattered_array_from_all (double* distdest, int x_dim_size, int y_dim_size, int z_dim_size,
 int distributed_dimension_id, int halo_size, int rank_id, int process_count, double** destinationDataPointer );


#ifdef __cplusplus
 }
#endif

#endif /* LIBXOMP_MPI_H */

