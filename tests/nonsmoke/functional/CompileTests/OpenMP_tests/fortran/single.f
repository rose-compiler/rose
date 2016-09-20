C***************************************************
C Liao, 1/20/2010
C***************************************************
      PROGRAM HELLO
      include 'omp_lib.h'
!$OMP PARALLEL
!$OMP SINGLE
      print *, 'Running on ', omp_get_num_threads(), 'threads'
!$omp END SINGLE
      PRINT *, 'Hello World!'
c$omp end parallel 
      PRINT *, 'the end'
      END
