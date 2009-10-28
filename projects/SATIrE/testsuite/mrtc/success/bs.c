/* MDH WCET BENCHMARK SUITE. File version $Id: bs.c,v 1.4 2005/12/14 14:44:47 jgn Exp $ */

/*************************************************************************/
/*                                                                       */
/*   SNU-RT Benchmark Suite for Worst Case Timing Analysis               */
/*   =====================================================               */
/*                              Collected and Modified by S.-S. Lim      */
/*                                           sslim@archi.snu.ac.kr       */
/*                                         Real-Time Research Group      */
/*                                        Seoul National University      */
/*                                                                       */
/*                                                                       */
/*        < Features > - restrictions for our experimental environment   */
/*                                                                       */
/*          1. Completely structured.                                    */
/*               - There are no unconditional jumps.                     */
/*               - There are no exit from loop bodies.                   */
/*                 (There are no 'break' or 'return' in loop bodies)     */
/*          2. No 'switch' statements.                                   */
/*          3. No 'do..while' statements.                                */
/*          4. Expressions are restricted.                               */
/*               - There are no multiple expressions joined by 'or',     */
/*                'and' operations.                                      */
/*          5. No library calls.                                         */
/*               - All the functions needed are implemented in the       */
/*                 source file.                                          */
/*                                                                       */
/*                                                                       */
/*************************************************************************/
/*                                                                       */
/*  FILE: bs.c                                                           */
/*  SOURCE : Public Domain Code                                          */
/*                                                                       */
/*  DESCRIPTION :                                                        */
/*                                                                       */
/*     Binary search for the array of 15 integer elements.               */
/*                                                                       */
/*  REMARK :                                                             */
/*                                                                       */
/*  EXECUTION TIME :                                                     */
/*                                                                       */
/*                                                                       */
/*************************************************************************/
/* Changes:
 * JG 2005/12/12: Prototypes added, printf removed, and changed exit to return in main.
 */
/*
#include<stdio.h>
*/
#define DEBUG

struct DATA {
	int             key;
	int             value;
};

#ifdef DEBUG
int             cnt1;
#endif

struct DATA     data[15] = {{1, 100},
{5, 200},
{6, 300},
{7, 700},
{8, 900},
{9, 250},
{10, 400},
{11, 600},
{12, 800},
{13, 1500},
{14, 1200},
{15, 110},
{16, 140},
{17, 133},
{18, 10}};

int             binary_search(int x);

int 
main(void)
{
	binary_search(8);
	return (0);
}

int 
binary_search(int x)
{
	int             fvalue, mid, up, low;

	low = 0;
	up = 14;
	fvalue = -1 /* all data are positive */ ;
	while (low <= up) {
		mid = (low + up) >> 1;
		if (data[mid].key == x) {	/* found  */
			up = low - 1;
			fvalue = data[mid].value;
#ifdef DEBUG
/*	printf("FOUND!!\n"); */
#endif
		} else
		 /* not found */ if (data[mid].key > x) {
			up = mid - 1;
#ifdef DEBUG
/*	printf("MID-1\n"); */
#endif
		} else {
			low = mid + 1;
#ifdef DEBUG
/*	printf("MID+1\n"); */
#endif
		}
#ifdef DEBUG
		cnt1++;
#endif
	}
#ifdef DEBUG
/*	printf("Loop Count : %d\n", cnt1); */
#endif
	return fvalue;
}
