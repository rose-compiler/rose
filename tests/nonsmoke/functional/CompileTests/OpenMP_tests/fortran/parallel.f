C***************************************************
C test the simplest helloworld in OpenMP Fortan
C comments are attached to real statements
C Liao, 9/30/2010
C***************************************************
      PROGRAM HELLO
!$OMP PARALLEL
      PRINT *, 'Hello World!'
c$omp end parallel 
      PRINT *, 'the end'
      END
