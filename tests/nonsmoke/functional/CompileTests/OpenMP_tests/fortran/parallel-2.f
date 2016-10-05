C***************************************************
C test the simple helloworld in OpenMP Fortan
C  nested parallel regions
C Liao, 9/30/2010
C***************************************************
      PROGRAM HELLO
!$OMP PARALLEL
      PRINT *, 'outer begin'
!$OMP PARALLEL
      PRINT *, 'Hello World!'
c$omp end parallel 
      PRINT *, ' outer end'
c$omp end parallel 
      END
