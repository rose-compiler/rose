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

#define OUTERREPS 20 
#define CONF95 1.96 

int nthreads, delaylength, innerreps; 
double times[OUTERREPS+1], reftime, refsd; 

double btest[IDA];
double atest[IDA];
   
#pragma omp threadprivate (btest)

   void refer(); 

   void testfirstprivnew(); 

   void testprivnew();

#ifdef OMPVER2
   void testcopyprivnew();   
#endif
   void testthrprivnew();   

   void stats(double*, double*); 


