C***************************************************
C Liao, 1/21/2010
C***************************************************
      PROGRAM HELLO
      include 'omp_lib.h'
      integer tid
      tid = 0
!$OMP PARALLEL
!$OMP MASTER
      tid = omp_get_thread_num()
      print *, 'Running on ', tid, 'threads'
      if (tid .ne. 0) call abort
!$omp END MASTER
      PRINT *, 'Hello World!'
c$omp end parallel 
      PRINT *, 'the end'
      END
