
#include "stdio.h"

#include "intone.h"

#define SIZE 10

// Transformed Code (Supporting function)

extern "C" void p_lu_r_01_l_01 ( int n, float c[SIZE][SIZE], int k );
void p_lu_r_01_l_01 ( int n, float c[SIZE][SIZE], int k )
   {
     int j, i, intone_start_02, intone_end_02, intone_last_02, intone_index_02;

     int intone_start = k+1;
	  int intone_end   = n;
	  int intone_step  = 1;
	  int intone_chunk = 0;
	  int intone_scheduletype = 0;

  // in__tone_begin_for_(k + 1,n,1,0,0);
     in__tone_begin_for_(&intone_start,&intone_end,&intone_step,&intone_chunk,&intone_scheduletype);
     while ( in__tone_next_iters_(&intone_start_02,&intone_end_02,&intone_last_02) == true )
        {
          for (i = intone_start_02; i <= intone_end_02; i++)
             {
               c[i][k] = c[i][k] / c[k][k];
               for (j = k + 1; j <= n; j++)
                  {
                    c[i][j] = c[i][j] - c[i][k] * c[k][j];
                  }
             }
        }

  // in__tone_end_for_(1);
     int intone_barrier_needed  = 1;
     in__tone_end_for_(&intone_barrier_needed);
   }

#if 0
// Put this into a separate file and compile it usingthe C compiler (gcc)
void p_lu_r_01 ( int* intone_me_01, int* intone_nprocs_01, int* intone_master01, int n, float c[SIZE][SIZE], int k )
   {
     p_lu_r_01_l_01(n,c,k);
   }
#endif

extern "C" void p_lu_r_01();
     
void main()
   {
     float c[SIZE][SIZE];
     int n;
     int k;

     int i, j;

     printf ("Inside of main() \n");

     int intone_lme_01, intone_master01, intone_me_01, intone_nprocs_01;
     intone_nprocs_01 = in__tone_cpus_current_();

     int intone_nargs = 04;
     int intone_nprocs = intone_nprocs_01;
     
  // in__tone_spawnparallel_( (void (*)()) p_lu_r_01,&intone_nargs,&intone_nprocs,&n,&c,&k);
     in__tone_spawnparallel_(p_lu_r_01,&intone_nargs,&intone_nprocs,&n,&c,&k);
   }

       
