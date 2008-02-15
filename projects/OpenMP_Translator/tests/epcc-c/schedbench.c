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
#include <math.h> 
#include <omp.h>  

#define MHZ 1500 
#define OUTERREPS 20 
#define CONF95 1.96 

   int nthreads, delaylength, innerreps, itersperthr, cksz; 
   double times[OUTERREPS+1], reftime, refsd; 

   void delay(int);
   void getdelay(void); 
   void refer(void); 
   void stats(double*, double*);
   void teststatic(void); 
   void teststaticn(void); 
   void testdynamicn(void);
   void testguidedn(void); 

int main (int argv, char **argc)
{
 

#pragma omp parallel
  { 
#pragma omp master 
    {
  nthreads = omp_get_num_threads(); 
    }
  } 
 
  printf("Running OpenMP benchmark on %d thread(s)\n", nthreads); 

  /* TUNE LENGTH OF LOOP BODY */ 
  getdelay(); 
  itersperthr = 1204; /*change from 128 to 256 for more test on guided,Liao*/
  innerreps = 1000;

  /* GENERATE REFERENCE TIME */ 
  refer();   

  /* TEST STATIC */ 
  teststatic(); 

  /* TEST STATIC,n */
  cksz = 1;
  while (cksz <= itersperthr){
    teststaticn();
    cksz *= 2;    
  }

  /* TEST DYNAMIC,n */

  cksz = 1;
  while (cksz <= itersperthr){
    testdynamicn();
    cksz *= 2;    
  }

  /* TEST GUIDED,n */
  cksz = 1;
  while (cksz <= itersperthr*2/nthreads){ /*increase cksz*/
    testguidedn();
    cksz *= 2;    
  }
  
}

void getdelay()
{
  int i,reps; 
  double actualtime, targettime, start; 

  double getclock(void); 

  /*  
      CHOOSE delaylength SO THAT call delay(delaylength) 
      TAKES APPROXIMATELY 100 CPU CLOCK CYCLES 
  */ 

  delaylength = 0;
  reps = 10000;

  actualtime = 0.;
  targettime = 100.0 / (double) MHZ * 1e-06;

  delay(delaylength); 

  while (actualtime < targettime) {
    delaylength = delaylength * 1.1 + 1; 
    start = getclock();
    for (i=0; i< reps; i++) {
      delay(delaylength); 
    }
    actualtime  = (getclock() - start) / (double) reps; 
  }

  start = getclock();
  for (i=0; i< reps; i++) {
    delay(delaylength); 
  }
  actualtime  = (getclock() - start) / (double) reps; 

  printf("Assumed clock rate = %d MHz \n",MHZ); 
  printf("Delay length = %d\n", delaylength); 
  printf("Delay time  = %f cycles\n",  actualtime * MHZ * 1e6); 

}

void refer()
{
  int i,j,k; 
  double start; 
  double meantime, sd; 

  double getclock(void); 

  printf("\n");
  printf("--------------------------------------------------------\n");
  printf("Computing reference time\n"); 

  for (k=0; k<=OUTERREPS; k++){
    start  = getclock(); 
    for (j=0; j<innerreps; j++){
      for (i=0; i<itersperthr; i++){
	delay(delaylength); 
      }
    }
    times[k] = (getclock() - start) * 1.0e6 / (double) innerreps;
  }

  stats (&meantime, &sd);

  printf("Reference_time_1 =                        %f microseconds +/- %f\n", meantime, CONF95*sd);

  reftime = meantime;
  refsd = sd;  
}

void teststatic()
{

  int i,j,k; 
  double start; 
  double meantime, sd; 

  double getclock(void); 

  printf("\n");
  printf("--------------------------------------------------------\n");
  printf("Computing STATIC time\n"); 

  for (k=0; k<=OUTERREPS; k++){
    start  = getclock(); 
#pragma omp parallel private(j) 
      { 
	for (j=0; j<innerreps; j++){
#pragma omp for schedule(static)  
	  for (i=0; i<itersperthr*nthreads; i++){
	    delay(delaylength); 
	  }
	}
      }
    times[k] = (getclock() - start) * 1.0e6 / (double) innerreps;
  }

  stats (&meantime, &sd);

  printf("STATIC time =                           %f microseconds +/- %f\n", meantime, CONF95*sd);
  
  printf("STATIC overhead =                       %f microseconds +/- %f\n", meantime-reftime, CONF95*(sd+refsd));

}

void teststaticn()
{

  int i,j,k; 
  double start; 
  double meantime, sd; 

  double getclock(void); 

  printf("\n");
  printf("--------------------------------------------------------\n");
  printf("Computing STATIC %d time\n",cksz); 

  for (k=0; k<=OUTERREPS; k++){
    start  = getclock(); 
#pragma omp parallel private(j) 
      { 
	for (j=0; j<innerreps; j++){
#pragma omp for schedule(static,cksz)  
	  for (i=0; i<itersperthr*nthreads; i++){
	    delay(delaylength); 
	  }
	}
      }
    times[k] = (getclock() - start) * 1.0e6 / (double) innerreps;
  }

  stats (&meantime, &sd);

  printf("STATIC %d time =                           %f microseconds +/- %f\n", cksz, meantime, CONF95*sd);
  
  printf("STATIC %d overhead =                       %f microseconds +/- %f\n", cksz, meantime-reftime, CONF95*(sd+refsd));

}

void testdynamicn()
{

  int i,j,k; 
  double start; 
  double meantime, sd; 

  double getclock(void); 

  printf("\n");
  printf("--------------------------------------------------------\n");
  printf("Computing DYNAMIC %d time\n",cksz); 

  for (k=0; k<=OUTERREPS; k++){
    start  = getclock(); 
#pragma omp parallel private(j) 
      { 
	for (j=0; j<innerreps; j++){
#pragma omp for schedule(dynamic,cksz)  
	  for (i=0; i<itersperthr*nthreads; i++){
	    delay(delaylength); 
	  }
	}
      }
    times[k] = (getclock() - start) * 1.0e6 / (double) innerreps;
  }

  stats (&meantime, &sd);

  printf("DYNAMIC %d time =                           %f microseconds +/- %f\n", cksz, meantime, CONF95*sd);
  
  printf("DYNAMIC %d overhead =                       %f microseconds +/- %f\n", cksz, meantime-reftime, CONF95*(sd+refsd));

}


void testguidedn()
{

  int i,j,k; 
  double start; 
  double meantime, sd; 

  double getclock(void); 

  printf("\n");
  printf("--------------------------------------------------------\n");
  printf("Computing GUIDED %d time\n",cksz); 

  for (k=0; k<=OUTERREPS; k++){
    start  = getclock(); 
#pragma omp parallel private(j) 
      { 
	for (j=0; j<innerreps; j++){
#pragma omp for schedule(guided,cksz)  
	  for (i=0; i<itersperthr*nthreads; i++){
	    delay(delaylength); 
	  }
	}
      }
    times[k] = (getclock() - start) * 1.0e6 / (double) innerreps;
  }

  stats (&meantime, &sd);

  printf("GUIDED %d time =                           %f microseconds +/- %f\n", cksz, meantime, CONF95*sd);
  
  printf("GUIDED %d overhead =                       %f microseconds +/- %f\n", cksz, meantime-reftime, CONF95*(sd+refsd));

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
  printf(" %d                %f   %f   %f    %f      %d\n",OUTERREPS, meantime, mintime, maxtime, sd, nr); 
  printf("\n");

  *mtp = meantime; 
  *sdp = sd; 

} 

