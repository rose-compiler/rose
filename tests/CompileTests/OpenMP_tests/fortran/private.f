C***************************************************
C test the simplest helloworld in OpenMP Fortan
C Liao, 12/7/2010
C***************************************************
      PROGRAM HELLO
      include 'omp_lib.h'

      INTEGER NTHREADS, TID
!$OMP PARALLEL PRIVATE(NTHREADS, TID)
      TID = OMP_GET_THREAD_NUM()
      PRINT *, 'Hello World by thread ', TID
c$omp end parallel 
      PRINT *, 'the end'
      END
