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

#define OUTERREPS 20 
#define CONF95 1.96 

   int nthreads, delaylength, innerreps; 
   double times[OUTERREPS+1], reftime, refsd; 

   void delay(int);
   void refer(void); 
   void referatom(void); 
   void referred(void); 

   void testpr(void); 
   void testfor(void); 
   void testpfor(void); 
   void testbar(void); 
   void testsing(void); 
   void testcrit(void); 
   void testlock(void); 
   void testorder(void); 
   void testatom(void); 
   void testred(void); 

   void stats(double*, double*); 

int main (int argv, char **argc)
{
 
#pragma omp parallel
  { 
#pragma omp master 
    {
  nthreads = omp_get_num_threads(); 
    }
  }
  
  printf(" Running OpenMP benchmark on %d thread(s)\n", nthreads); 


  delaylength = 500;
  innerreps = 10000;

  /* GENERATE REFERENCE TIME */ 
  refer();   

  /* TEST  PARALLEL REGION */ 
  innerreps = 1000;
  testpr(); 

  /* TEST  FOR */ 
  testfor(); 

  /* TEST  PARALLEL FOR */ 
  testpfor(); 

  /* TEST  BARRIER */ 
  testbar(); 

  /* TEST  SINGLE */ 
  testsing(); 

  /* TEST  CRITICAL*/ 
  innerreps = 100000;
  testcrit(); 

  /* TEST  LOCK/UNLOCK */ 
  testlock(); 

  /* TEST ORDERED SECTION */ 
  innerreps = 1000;
  testorder(); 

  /* GENERATE NEW REFERENCE TIME */ 
  innerreps = 100000; 
  referatom(); 

  /* TEST ATOMIC */ 
  testatom(); 

  /* GENERATE NEW REFERENCE TIME */ 
  innerreps = 10000; 
  referred(); 

  /* TEST REDUCTION (1 var)  */ 
  innerreps = 1000; 
  testred(); 

} 

void refer()
{
  int j,k; 
  double start; 
  double meantime, sd; 

  double getclock(void); 

  printf("\n");
  printf("--------------------------------------------------------\n");
  printf("Computing reference time 1\n"); 

  for (k=0; k<=OUTERREPS; k++){
    start  = getclock(); 
    for (j=0; j<innerreps; j++){
      delay(delaylength); 
    }
    times[k] = (getclock() - start) * 1.0e6 / (double) innerreps;
  }

  stats (&meantime, &sd);

  printf("Reference_time_1 =                        %f microseconds +/- %f\n", meantime, CONF95*sd);

  reftime = meantime;
  refsd = sd;  
}

void referatom()
{
  int j,k; 
  double start; 
  double meantime, sd; 
  float aaaa; 

  double getclock(void); 

  printf("\n");
  printf("--------------------------------------------------------\n");
  printf("Computing reference time 2\n"); 

  for (k=0; k<=OUTERREPS; k++){
    aaaa=0; 
    start  = getclock(); 
    for (j=0; j<innerreps; j++){
       aaaa += 1; 
    }
    times[k] = (getclock() - start) * 1.0e6 / (double) innerreps;
    if (aaaa < 0) printf("%f\n",aaaa); 
  }

  stats (&meantime, &sd);

  printf("Reference_time_2 =                        %f microseconds +/- %f\n", meantime, CONF95*sd);

  reftime = meantime;
  refsd = sd;  
}

void referred()
{
  int j,k; 
  double start; 
  double meantime, sd; 
  int aaaa; 

  double getclock(void); 

  printf("\n");
  printf("--------------------------------------------------------\n");
  printf("Computing reference time 3\n"); 

  for (k=0; k<=OUTERREPS; k++){
    aaaa=0; 
    start  = getclock(); 
    for (j=0; j<innerreps; j++){
       delay(delaylength); 
       aaaa += 1; 
    }
    times[k] = (getclock() - start) * 1.0e6 / (double) innerreps;
    if (aaaa < 0) printf("%d\n",aaaa); 
  }

  stats (&meantime, &sd);

  printf("Reference_time_3 =                        %f microseconds +/- %f\n", meantime, CONF95*sd);

  reftime = meantime;
  refsd = sd;  
}



void testpr()
{

  int j,k; 
  double start; 
  double meantime, sd; 

  double getclock(void); 

  printf("\n");
  printf("--------------------------------------------------------\n");
  printf("Computing PARALLEL time\n"); 

  for (k=0; k<=OUTERREPS; k++){
    start  = getclock(); 
    for (j=0; j<innerreps; j++){
#pragma omp parallel 
      {
      delay(delaylength); 
      }     
    }
    times[k] = (getclock() - start) * 1.0e6 / (double) innerreps;
  }

  stats (&meantime, &sd);

  printf("PARALLEL time =                           %f microseconds +/- %f\n", meantime, CONF95*sd);
  
  printf("PARALLEL overhead =                       %f microseconds +/- %f\n", meantime-reftime, CONF95*(sd+refsd));

}

void testfor()
{

  int i,j,k; 
  double start; 
  double meantime, sd; 

  double getclock(void); 

  printf("\n");
  printf("--------------------------------------------------------\n");
  printf("Computing FOR time\n"); 

  for (k=0; k<=OUTERREPS; k++){
    start  = getclock(); 
#pragma omp parallel private(j) 
      { 
	for (j=0; j<innerreps; j++){
#pragma omp for 
	  for (i=0; i<nthreads; i++){
	    delay(delaylength); 
	  }
	}
      }
    times[k] = (getclock() - start) * 1.0e6 / (double) innerreps;
  }

  stats (&meantime, &sd);

  printf("FOR time =                           %f microseconds +/- %f\n", meantime, CONF95*sd);
  
  printf("FOR overhead =                       %f microseconds +/- %f\n", meantime-reftime, CONF95*(sd+refsd));

}

void testpfor()
{

  int i,j,k; 
  double start; 
  double meantime, sd; 

  double getclock(void); 

  printf("\n");
  printf("--------------------------------------------------------\n");
  printf("Computing PARALLEL FOR time\n"); 

  for (k=0; k<=OUTERREPS; k++){
    start  = getclock(); 
    for (j=0; j<innerreps; j++){
#pragma omp parallel for 
      for (i=0; i<nthreads; i++){
	delay(delaylength); 
      }
    }
    times[k] = (getclock() - start) * 1.0e6 / (double) innerreps;
  }

  stats (&meantime, &sd);

  printf("PARALLEL FOR time =                           %f microseconds +/- %f\n", meantime, CONF95*sd);
  
  printf("PARALLEL FOR overhead =                       %f microseconds +/- %f\n", meantime-reftime, CONF95*(sd+refsd));

}

void testbar()
{

  int j,k; 
  double start; 
  double meantime, sd; 

  double getclock(void); 

  printf("\n");
  printf("--------------------------------------------------------\n");
  printf("Computing BARRIER time\n"); 

  for (k=0; k<=OUTERREPS; k++){
    start  = getclock(); 
#pragma omp parallel private(j)  
    {
      for (j=0; j<innerreps; j++){
	delay(delaylength); 
#pragma omp barrier 
      }     
    }
    times[k] = (getclock() - start) * 1.0e6 / (double) innerreps;
  }

  stats (&meantime, &sd);

  printf("BARRIER time =                           %f microseconds +/- %f\n", meantime, CONF95*sd);
  
  printf("BARRIER overhead =                       %f microseconds +/- %f\n", meantime-reftime, CONF95*(sd+refsd));

}

void testsing()
{

  int j,k; 
  double start; 
  double meantime, sd; 

  double getclock(void); 

  printf("\n");
  printf("--------------------------------------------------------\n");
  printf("Computing SINGLE time\n"); 

  for (k=0; k<=OUTERREPS; k++){
    start  = getclock(); 
#pragma omp parallel private(j) 
    {
      for (j=0; j<innerreps; j++){
#pragma omp single 
	delay(delaylength); 
      }     
    }
    times[k] = (getclock() - start) * 1.0e6 / (double) innerreps;
  }

  stats (&meantime, &sd);

  printf("SINGLE time =                           %f microseconds +/- %f\n", meantime, CONF95*sd);
  
  printf("SINGLE overhead =                       %f microseconds +/- %f\n", meantime-reftime, CONF95*(sd+refsd));

}

void testcrit()
{

  int j,k; 
  double start; 
  double meantime, sd; 

  double getclock(void); 

  printf("\n");
  printf("--------------------------------------------------------\n");
  printf("Computing CRITICAL time\n"); 

  for (k=0; k<=OUTERREPS; k++){
    start  = getclock(); 
#pragma omp parallel private(j) 
    {
      for (j=0; j<innerreps/nthreads; j++){
#pragma omp critical 
	{
	  delay(delaylength); 
	}
      }     
    }
    times[k] = (getclock() - start) * 1.0e6 / (double) innerreps;
  }

  stats (&meantime, &sd);

  printf("CRITICAL time =                           %f microseconds +/- %f\n", meantime, CONF95*sd);
  
  printf("CRITICAL overhead =                       %f microseconds +/- %f\n", meantime-reftime, CONF95*(sd+refsd));

}


void testlock()
{

  int j,k; 
  double start; 
  double meantime, sd; 
  omp_lock_t lock;

  double getclock(void); 

  printf("\n");
  printf("--------------------------------------------------------\n");
  printf("Computing LOCK/UNLOCK time\n"); 

  omp_init_lock(&lock); 
  for (k=0; k<=OUTERREPS; k++){
    start  = getclock(); 
#pragma omp parallel private(j) 
    {
      for (j=0; j<innerreps/nthreads; j++){
        omp_set_lock(&lock);
	delay(delaylength); 
	omp_unset_lock(&lock);
      }     
    }
    times[k] = (getclock() - start) * 1.0e6 / (double) innerreps;
  }

  stats (&meantime, &sd);

  printf("LOCK/UNLOCK time =                           %f microseconds +/- %f\n", meantime, CONF95*sd);
  
  printf("LOCK/UNLOCK overhead =                       %f microseconds +/- %f\n", meantime-reftime, CONF95*(sd+refsd));

}

void testorder()
{

  int j,k; 
  double start; 
  double meantime, sd; 

  double getclock(void); 

  printf("\n");
  printf("--------------------------------------------------------\n");
  printf("Computing ORDERED time\n"); 

  for (k=0; k<=OUTERREPS; k++){
    start  = getclock(); 
#pragma omp parallel for ordered schedule (static,1) 
    for (j=0; j<innerreps; j++){
#pragma omp ordered
      delay(delaylength); 
    }     
    times[k] = (getclock() - start) * 1.0e6 / (double) innerreps;
  }

  stats (&meantime, &sd);

  printf("ORDERED time =                           %f microseconds +/- %f\n", meantime, CONF95*sd);
  
  printf("ORDERED overhead =                       %f microseconds +/- %f\n", meantime-reftime, CONF95*(sd+refsd));

}

void testatom()
{

  int j,k; 
  double start; 
  double meantime, sd; 
  float aaaa;

  double getclock(void); 

  printf("\n");
  printf("--------------------------------------------------------\n");
  printf("Computing ATOMIC time\n"); 

  for (k=0; k<=OUTERREPS; k++){
    aaaa = 0; 
    start  = getclock(); 
#pragma omp parallel private(j) 
    {
      for (j=0; j<innerreps/nthreads; j++){
#pragma omp atomic 
	aaaa += 1; 
      }     
    }
    times[k] = (getclock() - start) * 1.0e6 / (double) innerreps;
    if (aaaa < 0.0) printf("%f\n",aaaa); 
  }

  stats (&meantime, &sd);

  printf("ATOMIC time =                           %f microseconds +/- %f\n", meantime, CONF95*sd);
  
  printf("ATOMIC overhead =                       %f microseconds +/- %f\n", meantime-reftime, CONF95*(sd+refsd));

}

void testred()
{

  int j,k; 
  double start; 
  double meantime, sd; 
  int aaaa;

  double getclock(void); 

  printf("\n");
  printf("--------------------------------------------------------\n");
  printf("Computing REDUCTION time\n"); 

  for (k=0; k<=OUTERREPS; k++){
    aaaa = 0; 
    start  = getclock(); 
    for (j=0; j<innerreps; j++){
#pragma omp parallel reduction(+:aaaa) 
      {
        delay(delaylength); 
	aaaa += 1; 
      }     
    }
    times[k] = (getclock() - start) * 1.0e6 / (double) innerreps;
    if (aaaa < 0) printf("%d\n",aaaa); 
  }

  stats (&meantime, &sd);

  printf("REDUCTION time =                           %f microseconds +/- %f\n", meantime, CONF95*sd);
  
  printf("REDUCTION overhead =                       %f microseconds +/- %f\n", meantime-reftime, CONF95*(sd+refsd));

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


