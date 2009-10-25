#include <upc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define NT 4 /* assumed number of threads */

/* function to check number of threads */
extern void check_thr(int num_thr);

/* function that returns an integer zero value which can not be calculated at compile time */
extern int zero();
