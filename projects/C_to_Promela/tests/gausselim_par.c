/*
 * Copyright (c) 2004, University of Massachusetts
 * All Rights Reserved
 *
 * Name:       gausselim_par.c
 * Date:       21 Jul 2004
 * Revised:    24 Jul 2004
 * Author:     Anastasia V. Mironova <mironova@laser.cs.umass.edu>
 * Author:     Stephen Siegel <siegel@cs.umass.edu>
 * Maintainer: Anastasia V. Mironova <mironova@laser.cs.umass.edu>
 * Reader:
 *
 * Compile:    mpicc gausselim_par.c
 * Run:        mpirun -np m a.out n m A[0,0] A[0,1] ... A[n-1,m-1]
 *
 *   n : number of rows in matrix
 *   m : number of columns in matrix
 *   A[0,0] .. A[n-1,m-1] : entries of matrix (doubles)
 *
 * Description: This is a parallel implementation of the Gauss-Jordan
 * elimination algorithm.  The input is an n x m matrix A of double
 * precision floating point numbers.  At termination, A has been
 * placed in reduced row-echelon form, where the rows are stored on
 * different processes. This implementation is based on the sequential
 * algorithm described in many places; see for example Howard Anton,
 * Elementary Linear Algebra, Wiley, 1977, Section 1.2.In this
 * implementation a modification to this algorithm has been made to
 * perform backward subsitution together with the process of reduction
 * to row-echelon form.
 *
 * The rows of the matrix are distributed among the processes so that
 * the process of rank i has the i-th row. For example, if A is the
 * 2x3 matrix
 * 
 *     A[0,0]   A[0,1]   A[0,2]
 *     A[1,0]   A[1,1]   A[1,2]
 *
 * process 0 has A[0,0], A[0,1], A[0,2], and process 1 has A[1,0],
 * A[1,1], A[1,2].  Naturally, the number of processes must equal the
 * number of rows.
 * 
 * Once the computation has taken place, the matrix is in the reduced
 * row-echelon form and distributed among the processes so that the
 * process with the lowest rank has the row with the left-most pivot
 * entry.
 */

#include "mpi.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Prints the given matrix of doubles to stdout. The parameter numRows
 * gives the number of rows in the matrix, and numCols the number of
 * columns.  This function uses MPI_Send and MPI_Recv to send all the
 * rows to process 0, which receives them in the proper order and
 * prints them out. The string message is printed out by process 0
 * first.
 */
void printMatrix(char* message, double* matrix, int numRows, int numCols) {
  int i,j;
  int rank;
  MPI_Status status;
  
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0) {
    double* row = (double*)malloc(numCols * sizeof(double));

    printf(message);
    for (i = 0; i < numRows; i++) {
      if (i == 0) {
	for (j = 0; j < numCols; j++) {
	  row[j] = matrix[j];
	}
      }
      else {
	MPI_Recv(row, numCols, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &status);
      }
      for (j = 0; j < numCols; j++) {
	printf("%lf ", row[j]);
      }
      printf("\n");
    }
    fprintf(stdout, "\n");
    fflush(stdout);
    free(row);
  }
  else {
    MPI_Send(matrix, numCols, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
  }
}


/* Performs Gaussian elimination on the given matrix of doubles.  The
 * parameter numRows gives the number of rows in the matrix, and
 * numCols the number of columns.  Upon return, the matrix will be in
 * reduced row-echelon form.
 */
int gausselim(double* matrix, int numRows, int numCols, int debug) {
  int top = 0;           // the current top row of the matrix
  int col = 0;           // column index of the current pivot
  int pivotRow = 0;      // row index of current pivot
  double pivot = 0.0;    // the value of the current pivot
  int j = 0;             // loop variable over columns of matrix
  double tmp = 0.0;      // temporary double variable
  MPI_Status status;     // status object needed for receives
  int rank;              // rank of this process
  int nprocs;            // number of processes
  double* toprow = (double*)malloc(numCols * sizeof(double));

  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
  for (top=col=0; top<numRows && col< numCols; top++, col++) {

    /* At this point we know that the submatrix consisting of the
     * first top rows of A is in reduced row-echelon form.  We will now
     * consider the submatrix B consisting of the remaining rows.  We
     * know, additionally, that the first col columns of B are
     * all zero.
     */

    if (debug && rank == 0) {
      printf("Top: %d\n", top);
    }

    /* Step 1: Locate the leftmost column of B that does not consist
     * of all zeros, if one exists.  The top nonzero entry of this
     * column is the pivot. */
  
    for (; col < numCols; col++) {
      if (matrix[col] != 0.0 && rank >= top) {
	MPI_Allreduce(&rank, &pivotRow, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
      }
      else {
	MPI_Allreduce(&nprocs, &pivotRow, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
      }
      if (pivotRow < nprocs){
	break;
      }
    }

    if (col >= numCols) {
      break;
    }

    if (debug) {
      if (rank == 0) {
	printf("Step 1 result: col=%d, pivotRow=%d\n\n", col, pivotRow);
      }
    }
      
    /* At this point we are guaranteed that pivot = A[pivotRow,col] is
     * nonzero.  We also know that all the columns of B to the left of
     * col consist entirely of zeros. */

    /* Step 2: Interchange the top row with the pivot row, if
     * necessary, so that the entry at the top of the column found in
     * Step 1 is nonzero. */

    if (pivotRow != top) {
      if (rank == top) {
	MPI_Sendrecv_replace(matrix, numCols, MPI_DOUBLE, pivotRow, 0, 
                             pivotRow, 0, MPI_COMM_WORLD, &status);
      }
      else if (rank == pivotRow) {
	MPI_Sendrecv_replace(matrix, numCols, MPI_DOUBLE, top, 0, 
                             top, 0,  MPI_COMM_WORLD, &status);
      }
    }

    if (rank == top) {
      pivot = matrix[col];
    }
    
    if (debug) {
      printMatrix("Step 2 result: \n", matrix, numRows, numCols);
    }

    /* At this point we are guaranteed that A[top,col] = pivot is
     * nonzero. Also, we know that (i>=top and j<col) implies
     * A[i,j] = 0. */

    /* Step 3: Divide the top row by pivot in order to introduce a
     * leading 1. */

    if (rank == top) {
      for (j = col; j < numCols; j++) {
	matrix[j] /= pivot;
	toprow[j] = matrix[j];
      }
    }

    if (debug) {
      printMatrix("Step 3 result:\n", matrix, numRows, numCols);
    }

    /* At this point we are guaranteed that A[top,col] is 1.0,
     * assuming that floating point arithmetic guarantees that a/a
     * equals 1.0 for any nonzero double a. */

    MPI_Bcast(toprow, numCols, MPI_DOUBLE, top, MPI_COMM_WORLD);

    /* Step 4: Add suitable multiples of the top row to rows below so
     * that all entries below the leading 1 become zero. */

    if (rank != top) {
      tmp = matrix[col];
      for (j = col; j < numCols; j++) {
	matrix[j] -= toprow[j]*tmp;
      }
    }

    if (debug) {
      printMatrix("Step 4 result: \n", matrix, numRows, numCols);
    }
  }
  free(toprow);
}



/*
 * Usage: mpirun -np m a.out debug n m A[0,0] A[0,1] ... A[n-1,m-1]
 *   
 *   debug : debug flag, triggers output to screen at every step if non-zero
 *   n : number of rows in matrix
 *   m : number of columns in matrix
 *   A[0,0] .. A[n-1,m-1] : entries of matrix (doubles)
 *
 * Computes reduced row-echelon form and prints intermediate steps.
 */
int main(int argc, char** argv) {
  int numRows;
  int numCols;
  int j;
  double* matrix;
  int rank, nprocs;
  int debug = 0;
  
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (argc < 3) {
    if (rank == 0) {
      printf("Too few arguments.\n");
      printf("Usage: ges [-debug] n m A[0,0] A[0,1] ... A[n-1,m-1]\n");
      printf("   n : number of rows in matrix\n");
      printf("   m : number of columns in matrix\n");
      printf("   A[0,0] .. A[n-1,m-1] : entries of matrix (doubles)\n");
    }
    exit(1);
  }
  if (!strcmp("-debug", argv[1])) {
    debug = 1;
  }
  sscanf(argv[1+debug], "%d", &numRows);
  sscanf(argv[2+debug], "%d", &numCols);
  if (argc != 3 + debug + numRows*numCols) {
    if (rank == 0) {
      printf("Incorrect number of matrix entries: %d expected, %d given.\n",
             numRows*numCols, argc-3-debug);
    }
    exit(1);
  }
  if (nprocs != numRows) {
    if (rank == 0) {
      printf("Number of processes must equal the number of rows: %d != %d \n",
             nprocs, numRows);
    }
    exit(1);
  }
  matrix = (double *) malloc(numCols * sizeof(double));
  for (j = 0; j < numCols; j++) {
    sscanf(argv[rank*numCols+j+3+debug], "%lf", &matrix[j]);
  }
  printMatrix("Original matrix:\n", matrix, numRows, numCols);
  gausselim(matrix, numRows, numCols, debug);
  printMatrix("Reduced row-echelon form:\n", matrix, numRows, numCols);
  MPI_Finalize();
}
