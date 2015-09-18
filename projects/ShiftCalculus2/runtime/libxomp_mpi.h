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
#include <assert.h>
#include <sys/time.h>
#include <string.h> // for memcpy()

/* prototype declarations */

/*Initialize MPI and obtain default rank and number of processes*/
extern void xomp_init_mpi (int* argc, char** argv[], int * rank, int * nproc); 

#ifdef __cplusplus
 }
#endif

#endif /* LIBXOMP_MPI_H */

