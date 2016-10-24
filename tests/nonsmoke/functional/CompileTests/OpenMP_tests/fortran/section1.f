      PROGRAM HELLO
!$OMP PARALLEL
!$omp single
      PRINT *, 'Hello World from single'
!$omp end single
!$omp sections 
!$omp section
      PRINT *, 'Hello World from section 1'
!$omp section
      PRINT *, 'Hello World from section 2'
!$omp end sections nowait
c$omp end parallel 
      PRINT *, 'the end'
      END

