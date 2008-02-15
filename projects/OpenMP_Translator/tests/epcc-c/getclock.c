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
#ifdef __cplusplus
extern "C" {
#endif
      double omp_get_wtime(void);
#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif


static int firstcall = 1; 

double getclock()
{
      double time;
#ifdef OMPCLOCK

      time = omp_get_wtime();
      return time;
#endif

#ifdef GTODCLOCK
      double get_time_of_day_(void);  
      void init_time_of_day_(void);      

      if (firstcall) {
         init_time_of_day_(); 
         firstcall = 0;
      }
      time = get_time_of_day_(); 
      return time;
#endif

} 
