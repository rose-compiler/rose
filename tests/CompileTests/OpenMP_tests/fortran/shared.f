C***************************************************
C test the simplest helloworld in OpenMP Fortan
C Liao, 12/7/2010
C***************************************************
      PROGRAM HELLO
      include 'omp_lib.h'

      INTEGER TID
      INTEGER S
      PARAMETER (S=888)
      REAL K
      K = 0.5
!$OMP PARALLEL PRIVATE(TID)
      TID = OMP_GET_THREAD_NUM()
      PRINT *, 'Hello World by thread ', TID, ' shared variables ', S, K
c$omp end parallel 
      PRINT *, 'the end'
      END
