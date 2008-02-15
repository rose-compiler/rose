/*

INTONE STYLE CODE GENERATION

c
c lu - LU computation
c
	subroutine lu (C,N)
	implicit none
	integer*4 N
	real C(N, N)

	integer*4 i, j, k

 	do k = 1, N-1
C$OMP PARALLEL DO PRIVATE(i,j) SCHEDULE(STATIC)
	   do i = k+1, N
	      C(i,k) = C(i,k) / C(k,k)
	      do j = k+1, N
	         C(i,j) = C(i,j) - C(i,k)*C(k,j)
	      enddo
	   enddo
 	enddo
	end

// Transformed Code

       SUBROUTINE lu(c,n)
       IMPLICIT NONE
       EXTERNAL p_lu_r_01
       INTEGER intone_lme_01, intone_master01, intone_me_01, intone_npro
     1cs_01
       INTEGER n
       REAL c(n,n)
       INTEGER i, j, k
       INTEGER in__tone_cpus_current
       EXTERNAL in__tone_cpus_current
       
       DO k = 1,n - 1
         intone_nprocs_01 = in__tone_cpus_current()
         CALL in__tone_spawnparallel(p_lu_r_01,04,intone_nprocs_01,n,c,k
     1)
       END DO
       END
       

// Transformed Code (Supporting function)

       SUBROUTINE p_lu_r_01(intone_me_01,intone_nprocs_01,intone_master01,n,c,k)
       IMPLICIT NONE
       INTEGER intone_me_01, intone_nprocs_01, intone_master01, n
       REAL c(n,n)
       INTEGER k
       INTEGER i, j
       
       CALL p_lu_r_01_l_01(n,c,k)
       RETURN 
       END       

// Transformed Code (Supporting function)

       SUBROUTINE p_lu_r_01_l_01(n,c,k)
       IMPLICIT NONE
       INTEGER n
       REAL c(n,n)
       INTEGER k
       INTEGER j, i, intone_start_02, intone_end_02, intone_last_02, intone_index_02
       EXTERNAL in__tone_spawnparallel
       EXTERNAL in__tone_begin_for
       EXTERNAL in__tone_end_for
       LOGICAL in__tone_next_iters
       EXTERNAL in__tone_next_iters
       EXTERNAL in__tone_barrier
       LOGICAL in__tone_is_master
       EXTERNAL in__tone_is_master
       EXTERNAL in__tone_set_lock
       EXTERNAL in__tone_unset_lock
       EXTERNAL in__tone_enter_ordered
       EXTERNAL in__tone_leave_ordered
       EXTERNAL in__tone_global_flush
       EXTERNAL in__tone_selective_flush
       INTEGER in__tone_atomic_update_4
       EXTERNAL in__tone_atomic_update_4
       INTEGER *8 in__tone_atomic_update_8
       EXTERNAL in__tone_atomic_update_8
       INTEGER in__tone_cpus_current
       EXTERNAL in__tone_cpus_current
       INTEGER in__tone_thread_id
       EXTERNAL in__tone_thread_id
       
       CALL in__tone_begin_for(k + 1,n,1,0,0)
       DO WHILE (.TRUE. .EQV. in__tone_next_iters(intone_start_02,intone_end_02,intone_last_02))
         DO i = intone_start_02,intone_end_02
           c(i,k) = c(i,k) / c(k,k)
           DO j = k + 1,n
             c(i,j) = c(i,j) - c(i,k) * c(k,j)
           END DO
         END DO
       END DO
       CALL in__tone_end_for(1)
       RETURN 
       END
*/

// Example original code with OpenMP directive
void lu ( float c[SIZE][SIZE], int n )
   {
     int i, j, k;
 	  for (k = 1, k <= SIZE-1; k++)
        {
#pragma omp parallel for private(i,j) schedule(static)
          for (i = k+1, i <= SIZE; i++)
             {
               c[i][k] = c[i][k] / c[k][k];
               for (j = k+1; k <= SIZE; j++)
                  {
	                 c[i][j] = c[i][j] - c[i][k]*c[k][j];
                  }
             }
        }
   }


// Transformed Code

#include "intone.h"

void lu ( float c[SIZE][SIZE], int n )
   {
     void p_lu_r_01();

     int i, j, k;
     int intone_lme_01, intone_master01, intone_me_01, intone_nprocs_01;
       
     for (k = 1; k <= n - 1; k++)
        {
          intone_nprocs_01 = in__tone_cpus_current();
          in__tone_spawnparallel(p_lu_r_01,04,intone_nprocs_01,n,c,k);
        }
   }

       
// Transformed Code (Supporting function)

void p_lu_r_01 ( int intone_me_01, int intone_nprocs_01, int intone_master01, int n, float c[SIZE][SIZE], int k )
   {
     p_lu_r_01_l_01(n,c,k);
   }

// Transformed Code (Supporting function)

void p_lu_r_01_l_01 ( int n, float c[SIZE][SIZE], int k )
   {
     int j, i, intone_start_02, intone_end_02, intone_last_02, intone_index_02;

     in__tone_begin_for(k + 1,n,1,0,0);
     while ( in__tone_next_iters(intone_start_02,intone_end_02,intone_last_02) == true )
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

     in__tone_end_for(1);
   }
          
