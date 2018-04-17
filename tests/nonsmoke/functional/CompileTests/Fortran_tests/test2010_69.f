!     test comment before 'END'      
!     Liao 10/5/2010
      SUBROUTINE HELLO
c$OMP PARALLEL
      PRINT *, 'Hello World!'
c$OMP END PARALLEL
      END
!     oops , how about this line!!
