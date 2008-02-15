/***************************************************************************
*                                                                         *
*             OpenMP MicroBenchmark Suite - Version 2.0                   *
*                                                                         *
*                            produced by                                  *
*                                                                         *
*                     Mark Bull and Fiona Reid                            *
*                                                                         *
*                                at                                       *
*                                                                         *
*                Edinburgh Parallel Computing Centre                      *
*                                                                         *
*         email: markb@epcc.ed.ac.uk or fiona@epcc.ed.ac.uk               *
*                                                                         *
*                                                                         *
*      This version copyright (c) The University of Edinburgh, 2004.      *
*                         All rights reserved.                            *
*                                                                         *
**************************************************************************/



#include <stdio.h> 
#include <stdlib.h>
#include <math.h> 
#include <omp.h>  
#include "arraybench.h"

void delay(int, double*);
int main (int argv, char **argc)
{
  nthreads = 1; 
#pragma omp parallel
  { 
#pragma omp master 
    {
#ifdef _OPENMP
  nthreads = omp_get_num_threads(); 
#endif 
    }
  }

  printf(" Running OpenMP benchmark on %d thread(s)\n", nthreads); 

  delaylength = 500;
  innerreps = 100;
  /* GENERATE REFERENCE TIME */ 
  refer();   

  /* TEST  PRIVATE */ 
   testprivnew(); 

  /* TEST  FIRSTPRIVATE */ 
  testfirstprivnew(); 

#ifdef OMPVER2
  /* TEST  COPYPRIVATE */ 
  testcopyprivnew(); 
#endif

  /* TEST  THREADPRIVATE - COPYIN */ 
  testthrprivnew(); 

  delaylength = 500;
  innerreps = 100;

} 

void refer()
{
  int j,k; 
  double start; 
  double meantime, sd; 
  double a[1];
  double getclock(void); 

  printf("\n");
  printf("--------------------------------------------------------\n");
  printf("Computing reference time 1\n"); 

  for (k=0; k<=OUTERREPS; k++){
    start  = getclock(); 
    for (j=0; j<innerreps; j++){
      delay(delaylength, a); 
    }
    times[k] = (getclock() - start) * 1.0e6 / (double) innerreps;
  }

  stats (&meantime, &sd);

  printf("Reference_time_1 =                        %10.3f microseconds +/- %10.3f\n", meantime, CONF95*sd);

  reftime = meantime;
  refsd = sd;  
}


void testfirstprivnew()
{

  int n,j,k; 
  double start; 
  double meantime, sd; 
  double getclock(void); 

  n=IDA;
  printf("\n");
  printf("--------------------------------------------------------\n");
  printf("Computing FIRSTPRIVATE %d time\n", n); 

  for (k=0; k<=OUTERREPS; k++){
    start  = getclock(); 
    for (j=0; j<innerreps; j++){
#pragma omp parallel firstprivate(atest) 
      {
      delay(delaylength, atest); 
      }     
    }
    times[k] = (getclock() - start) * 1.0e6 / (double) innerreps;
  }

  stats (&meantime, &sd);

  printf("FIRSTPRIVATE time =                           %10.3f microseconds +/- %10.3f\n", meantime, CONF95*sd);  
  printf("FIRSTPRIVATE overhead =                       %10.3f microseconds +/- %10.3f\n", meantime-reftime, CONF95*(sd+refsd));

}

void testprivnew()
{

  int n,j,k; 
  double start; 
  double meantime, sd; 
  double getclock(void); 

  n=IDA;
  printf("\n");
  printf("--------------------------------------------------------\n");
  printf("Computing PRIVATE %d time\n", n); 

  for (k=0; k<=OUTERREPS; k++){
    start  = getclock(); 
    for (j=0; j<innerreps; j++){
#pragma omp parallel private(atest) 
      {
      delay(delaylength, atest); 
      }     
    }
    times[k] = (getclock() - start) * 1.0e6 / (double) innerreps;
  }

  stats (&meantime, &sd);

  printf("PRIVATE time =                           %10.3f microseconds +/- %10.3f\n", meantime, CONF95*sd);  
  printf("PRIVATE overhead =                       %10.3f microseconds +/- %10.3f\n", meantime-reftime, CONF95*(sd+refsd));

}

#ifdef OMPVER2
void testcopyprivnew()
{

  int n,j,k; 
  double start; 
  double meantime, sd; 
  double getclock(void); 
  n=IDA;
  printf("\n");
  printf("--------------------------------------------------------\n");
  printf("Computing COPYPRIVATE %d time\n", n); 

  for (k=0; k<=OUTERREPS; k++){
    start  = getclock(); 
    for (j=0; j<innerreps; j++){
#pragma omp single copyprivate(btest) 
      {
      delay(delaylength, btest); 
      }     
    }
    times[k] = (getclock() - start) * 1.0e6 / (double) innerreps;
  }

  stats (&meantime, &sd);

  printf("COPYPRIVATE time =                           %10.3f microseconds +/- %10.3f\n", meantime, CONF95*sd);
  printf("COPYPRIVATE overhead =                       %10.3f microseconds +/- %10.3f\n", meantime-reftime, CONF95*(sd+refsd));

}
#endif

void testthrprivnew()
{

  int n,j,k; 
  double start; 
  double meantime, sd; 
  double getclock(void); 
  n=IDA;
  printf("\n");
  printf("--------------------------------------------------------\n");
  printf("Computing COPYIN %d time\n", n); 

  for (k=0; k<=OUTERREPS; k++){
    start  = getclock(); 
    for (j=0; j<innerreps; j++){
#pragma omp parallel copyin(btest) 
      {
      delay(delaylength, btest); 
      }     
    }
    times[k] = (getclock() - start) * 1.0e6 / (double) innerreps;
  }

  stats (&meantime, &sd);

  printf("COPYIN time =                           %10.3f microseconds +/- %10.3f\n", meantime, CONF95*sd);
  printf("COPYIN overhead =                       %10.3f microseconds +/- %10.3f\n", meantime-reftime, CONF95*(sd+refsd));

}


void stats (double *mtp, double *sdp) 
{

  double meantime, totaltime, sumsq, mintime, maxtime, sd, cutoff; 

  int i, nr; 

  mintime = 1.0e10;
  maxtime = 0.;
  totaltime = 0.;

  for (i=1; i<=OUTERREPS; i++){
    mintime = (mintime < times[i]) ? mintime : times[i];
    maxtime = (maxtime > times[i]) ? maxtime : times[i];
    totaltime +=times[i]; 
  } 

  meantime  = totaltime / OUTERREPS;
  sumsq = 0; 

  for (i=1; i<=OUTERREPS; i++){
    sumsq += (times[i]-meantime)* (times[i]-meantime); 
  } 
  sd = sqrt(sumsq/(OUTERREPS-1));

  cutoff = 3.0 * sd; 

  nr = 0; 
  
  for (i=1; i<=OUTERREPS; i++){
    if ( fabs(times[i]-meantime) > cutoff ) nr ++; 
  }
  
  printf("\n"); 
  printf("Sample_size       Average     Min         Max          S.D.          Outliers\n");
  printf(" %d           %10.3f   %10.3f   %10.3f    %10.3f      %d\n",OUTERREPS, meantime, mintime, maxtime, sd, nr); 
  printf("\n");

  *mtp = meantime; 
  *sdp = sd; 

} 
