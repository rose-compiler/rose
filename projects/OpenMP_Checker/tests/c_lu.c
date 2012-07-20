/*************************************************************************
  This program is part of the
	OpenMP Source Code Repository

	http://www.pcg.ull.es/ompscr/
	e-mail: ompscr@etsii.ull.es

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License 
  (LICENSE file) along with this program; if not, write to
  the Free Software Foundation, Inc., 59 Temple Place, Suite 330, 
  Boston, MA  02111-1307  USA
	
FILE:		c_lu.c
VERSION:	1.0
DATE:
AUTHOR:		Arturo González-Escribano
COMMENTS TO:	arturo@infor.uva.es
DESCRIPTION:       
		LU reduction of a 2D dense matrix

COMMENTS:        
REFERENCES:     
BASIC PRAGMAS:	parallel-for (stride scheduling)
USAGE: 		./c_lu.par <size>
INPUT:		The matrix has fixed innitial values: 
		M[i][j] = 1.0	                   iff (i==j)
		M[i][j] = 1.0 + (i*numColums)+j    iff (i!=j)

OUTPUT:		Compile with -DDEBUG to see final matrix values
FILE FORMATS:
RESTRICTIONS:
REVISION HISTORY:
**************************************************************************/

/* the expected result is there is no race condition need to be reported, since SMT solver can distinguish iteration for loop in parallel region */

#include<stdio.h>
#include<stdlib.h>
#include "OmpSCR.h"


/* PROTOYPES */
void lu(int, int);


/* MAIN: PROCESS PARAMETERS */
int main(int argc, char *argv[]) {
int nthreads, size;
char *argNames[1] = { "size" };
char *defaultValues[1] = { "500" };
char *timerNames[1] = { "EXE_TIME" };

nthreads = omp_get_max_threads();
OSCR_init( nthreads,
	"LU reduction of a dense matrix.",
	NULL,
	1,
	argNames,
	defaultValues,
	1,
	1,
	timerNames,
	argc,
	argv );

/* 1. GET PARAMETERS */
size = OSCR_getarg_int(1);

/* 2. CALL COMPUTATION */
lu(nthreads, size);

/* 3. REPORT */
OSCR_report();


return 0;
}


/*
* LU FORWARD REDUCTION
*/
void lu(int nthreads, int size) {
/* DECLARE MATRIX AND ANCILLARY DATA STRUCTURES */
double **M;
double **L;

/* VARIABLES */
int i,j,k;

/* 0. ALLOCATE MATRICES MEMORY */
M = (double **)OSCR_calloc(size, sizeof(double *));
L = (double **)OSCR_calloc(size, sizeof(double *));
for (i=0; i<size; i++) {
	M[i] = (double *)OSCR_calloc(size, sizeof(double));
	L[i] = (double *)OSCR_calloc(size, sizeof(double));
	}

/* 1. INITIALIZE MATRIX */
for (i=0; i<size; i++) {
	for (j=0; j<size; j++) {
		if (i==j) M[i][j]=1.0;
		else M[i][j]=1.0+(i*size)+j; 
		L[i][j]=0.0;
		}
	}

/* 3. START TIMER */
OSCR_timer_start(0);

/* 4. ITERATIONS LOOP */
for(k=0; k<size-1; k++) {

	/* 4.1. PROCESS ROWS IN PARALLEL, DISTRIBUTE WITH nthreads STRIDE */
#pragma omp parallel default(none) shared(M,L,size,k) private(i,j)
       #pragma omp for
 	for (i=k+1; i<size; i++) {
		/* 4.1.1. COMPUTE L COLUMN */
		L[i][k+ 1] = M[i][k] / M[k][k];

		/* 4.1.2. COMPUTE M ROW ELEMENTS */
		for (j=k+1; j<size; j++) 
			M[i][j] = M[i][j] - L[i][k]*M[k][j];
		}

/* 4.2. END ITERATIONS LOOP */
	}


/* 5. STOP TIMER */
OSCR_timer_stop(0);

/* 6. WRITE MATRIX (DEBUG) */
#ifdef DEBUG
#include "debug_ML.c"
#endif

/* 7. END */
}

