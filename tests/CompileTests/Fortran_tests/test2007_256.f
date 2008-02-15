! It is similar to the one I reported before. But this time CONTAINS is
! used in main program, not in a module.
! ------------input-----------------
      PROGRAM MAIN
      IMPLICIT NONE
      INTEGER i
      CONTAINS
        SUBROUTINE TEST(i)
        REAL :: i
        END SUBROUTINE
      END
