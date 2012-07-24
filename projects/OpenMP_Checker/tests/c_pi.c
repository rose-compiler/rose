/* ***********************************************************************
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
	
	FILE:              c_pi.c
  VERSION:           1.0
  DATE:              May 2004
  COMMENTS TO:       sande@csi.ull.es
  DESCRIPTION:       Parallel implementation of PI generator using OpenMP
  COMMENTS:          The area under the curve y=4/(1+x*x) between 0 and 1 provides a way to compute Pi
	                   The value of this integral can be approximated using a sum.
  REFERENCES:        http://en.wikipedia.org/wiki/Pi
	                   http://nereida.deioc.ull.es/~llCoMP/examples/examples/pi/pi_description.html
  BASIC PRAGMAS:     parallel
  USAGE:             ./c_pi.par
  INPUT:             Default precision
  OUTPUT:            The value of PI
	FILE FORMATS:      -
	RESTRICTIONS:      -
	REVISION HISTORY:
**************************************************************************/
/*

 variable local should have been in private clauses, otherwise it can cause race condition
 the expected result is that local  has been reported as data race 

 */



#include "OmpSCR.h"

#define DEFAULT_PREC 1000000   /* Default precision */
#define NUM_ARGS	1
#define NUM_TIMERS  	1

int main(int argc, char *argv[]) {
  double PI25DT = 3.141592653589793238462643;
  double local, w, total_time, pi;
  long i, 
	     N;   /* Precision */
  int NUMTHREADS;
	char *PARAM_NAMES[NUM_ARGS] = {"Precision"};
	char *DEFAULTS_VALUE[NUM_ARGS] = {"1000000"};
	char *TIMERS_NAMES[NUM_TIMERS] = {"Total_time"};

	/* Default: DEFAULT_PREC; */

	NUMTHREADS = omp_get_max_threads();
	OSCR_init (NUMTHREADS, "Pi generator", "Param: precission", NUM_ARGS, PARAM_NAMES, DEFAULTS_VALUE , NUM_TIMERS, NUM_TIMERS, TIMERS_NAMES, argc, argv); 
	N = OSCR_getarg_int(NUM_ARGS);
	OSCR_timer_start(0);
  w = 1.0 / N;
  pi = 0.0;
/* #pragma omp parallel for default(shared) private(i, local)reduction(+:pi) schedule(static, 1) */
#pragma omp parallel default(shared)
    #pragma omp for reduction(+:pi)
  for(i = 0; i < N; i++) {
    local = (i + 0.5) * w;
    pi += 4.0 / (1.0 + local * local);
  }
  pi *= w;
	OSCR_timer_stop(0);
	total_time = OSCR_timer_read(0);
  
  OSCR_report(); 
  printf("\n \t# THREADS INTERVAL \tTIME (secs.) \tPI \t\t\tERROR\n");
  printf("\t %d \t%10ld \t%14.6lf \t%1.20f\t%g\n", NUMTHREADS, N, total_time, pi, PI25DT-pi);
  return 0;
}

/*
 * vim:ts=2:sw=2:
 */

