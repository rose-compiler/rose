C***************************************************
C test the simplest helloworld in OpenMP Fortan
C Liao, 5/24/2009
C***************************************************
      PROGRAM HELLO
!$OMP PARALLEL 
      PRINT *, 'Hello World!'
c$OMP END PARALLEL
      END
