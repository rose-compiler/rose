/* MDH WCET BENCHMARK SUITE. File version $Id: matmult.c,v 1.3 2005/11/11 10:31:26 ael01 Exp $ */

/* matmult.c */
/* was mm.c! */


/*----------------------------------------------------------------------*
 * To make this program compile under our assumed embedded environment,
 * we had to make several changes:
 * - Declare all functions in ANSI style, not K&R.
 *   this includes adding return types in all cases!
 * - Declare function prototypes
 * - Disable all output
 * - Disable all UNIX-style includes
 *
 * This is a program that was developed from mm.c to matmult.c by
 * Thomas Lundqvist at Chalmers.
 *----------------------------------------------------------------------*/


/* Changes:
 * JG 2005/12/12: Indented program.
 */
 
#define UPPSALAWCET 1


/* ***UPPSALA WCET***:
   disable stupid UNIX includes */
#ifndef UPPSALAWCET
#include <sys/types.h>
#include <sys/times.h>
#endif

/*
 * MATRIX MULTIPLICATION BENCHMARK PROGRAM:
 * This program multiplies 2 square matrices resulting in a 3rd
 * matrix. It tests a compiler's speed in handling multidimensional
 * arrays and simple arithmetic.
 */

#define UPPERLIMIT 20

typedef int     matrix[UPPERLIMIT][UPPERLIMIT];

int             Seed;
matrix          ArrayA, ArrayB, ResultArray;

#ifdef UPPSALAWCET
/* Our picky compiler wants prototypes! */
void            Multiply(matrix A, matrix B, matrix Res);
void            InitSeed(void);
void            Test(matrix A, matrix B, matrix Res);
void            Initialize(matrix Array);
int             RandomInteger(void);
#endif

int 
main(void)
{
	InitSeed();
/* ***UPPSALA WCET***:
   no printing please! */
#ifndef UPPSALAWCET
	printf("\n   *** MATRIX MULTIPLICATION BENCHMARK TEST ***\n\n");
	printf("RESULTS OF THE TEST:\n");
#endif
	Test(ArrayA, ArrayB, ResultArray);
	return 0;
}


void 
InitSeed(void)
/*
 * Initializes the seed used in the random number generator.
 */
{
	/*
	 * ***UPPSALA WCET***: changed Thomas Ls code to something simpler.
	 * Seed = KNOWN_VALUE - 1;
	 */
	Seed = 0;
}


void 
Test(matrix A, matrix B, matrix Res)
/*
 * Runs a multiplication test on an array.  Calculates and prints the
 * time it takes to multiply the matrices.
 */
{
#ifndef UPPSALAWCET
	long            StartTime, StopTime;
	float           TotalTime;
#endif

	Initialize(A);
	Initialize(B);

	/* ***UPPSALA WCET***: don't print or time */
#ifndef UPPSALAWCET
	StartTime = ttime();
#endif

	Multiply(A, B, Res);

	/* ***UPPSALA WCET***: don't print or time */
#ifndef UPPSALAWCET
	StopTime = ttime();
	TotalTime = (StopTime - StartTime) / 1000.0;
	printf("    - Size of array is %d\n", UPPERLIMIT);
	printf("    - Total multiplication time is %3.3f seconds\n\n", TotalTime);
#endif
}


void 
Initialize(matrix Array)
/*
 * Intializes the given array with random integers.
 */
{
	int             OuterIndex, InnerIndex;

	for (OuterIndex = 0; OuterIndex < UPPERLIMIT; OuterIndex++)
		for (InnerIndex = 0; InnerIndex < UPPERLIMIT; InnerIndex++)
			Array[OuterIndex][InnerIndex] = RandomInteger();
}


int 
RandomInteger(void)
/*
 * Generates random integers between 0 and 8095
 */
{
	Seed = ((Seed * 133) + 81) % 8095;
	return (Seed);
}


#ifndef UPPSALAWCET
int 
ttime()
/*
 * This function returns in milliseconds the amount of compiler time
 * used prior to it being called.
 */
{
	struct tms      buffer;
	int             utime;

	/* times(&buffer);   times not implemented */
	utime = (buffer.tms_utime / 60.0) * 1000.0;
	return (utime);
}
#endif

void 
Multiply(matrix A, matrix B, matrix Res)
/*
 * Multiplies arrays A and B and stores the result in ResultArray.
 */
{
	register int    Outer, Inner, Index;

	for (Outer = 0; Outer < UPPERLIMIT; Outer++)
		for (Inner = 0; Inner < UPPERLIMIT; Inner++) {
			Res[Outer][Inner] = 0;
			for (Index = 0; Index < UPPERLIMIT; Index++)
				Res[Outer][Inner] +=
					A[Outer][Index] * B[Index][Inner];
		}
}
