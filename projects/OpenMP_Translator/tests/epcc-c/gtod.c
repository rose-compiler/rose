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
#include <stdio.h> /*add by Liao for undefined NULL*/
#include <sys/time.h>
extern double get_time_of_day_(void);
extern void init_time_of_day_(void);

time_t starttime = 0; 

double get_time_of_day_()
{

  struct timeval ts; 

  double t;

  int err; 

  err = gettimeofday(&ts, NULL); 

  t = (double) (ts.tv_sec - starttime)  + (double) ts.tv_usec * 1.0e-6; 
 
  return t; 

}

void init_time_of_day_()
{
  struct  timeval  ts;
  int err; 

  err = gettimeofday(&ts, NULL);
  starttime = ts.tv_sec; 
}


