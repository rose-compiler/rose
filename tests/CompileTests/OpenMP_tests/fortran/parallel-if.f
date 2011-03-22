C***************************************************
C test if clause
C Liao, 3/17/2011
C***************************************************
      PROGRAM HELLO
      include "omp_lib.h"
      integer i
      integer tcount
      i = 0
!$OMP PARALLEL if( i .eq. 0)
      PRINT *, 'Hello World!'
c$omp end parallel 

!$OMP PARALLEL if( i .ne. 0)
      PRINT *, 'Hello World 2!'
!$omp single
      tcount = omp_get_num_threads ()
      print *, tcount
      if (tcount .ne. 1 ) call abort
!$omp end single

c$omp end parallel 
      PRINT *, 'the end'
      END
