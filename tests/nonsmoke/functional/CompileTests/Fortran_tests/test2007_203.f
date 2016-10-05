! This is another test code that demonstrates that the implicit function is not finished!

! This is not allowed here!
!     IMPLICIT DOUBLE PRECISION(A-H,O-Z)

      SUBROUTINE TASSIG(PI,TEST)

!     This is allowed in the subroutine
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)

! If there is an implicit statement for the suroutine and there are function parameters, 
! then they must be explicitly typed or declared after the implicit type statement.
      INTEGER PI,TEST

      TEST = 2.0* PI
      END

!      PROGRAM TASSIG
!      PARAMETER (PI=3.14159265)
!      TEST = 2.0* PI
!      WRITE(*,*) TEST
      END
