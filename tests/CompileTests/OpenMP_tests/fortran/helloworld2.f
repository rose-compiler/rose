C***************************************************
C test the simplest helloworld in OpenMP Fortan
C  note the different sentinels and lower/upper cases
C note the contatenation for endparallel
C Liao, 5/24/2009
C***************************************************
      PROGRAM HELLO
!$OMP PARALLEL
      PRINT *, 'Hello World!'
c$omp endparallel 
      PRINT *, 'the end'
      END
