! Type Declaration
      CHARACTER(KIND=1,LEN=6) APPLES /'APPLES'/
      CHARACTER*7, TARGET :: ORANGES = 'ORANGES'
      CALL TEST(APPLES)
      END

      SUBROUTINE  TEST(VARBL)
        CHARACTER*(*), OPTIONAL :: VARBL   ! VARBL inherits a length of 6

        COMPLEX, DIMENSION (2,3) :: ABC(3) ! ABC has 3 (not 6) array elements
        REAL, POINTER :: XCONST

        TYPE PEOPLE                        ! Defining derived type PEOPLE
          INTEGER AGE
          CHARACTER*20 NAME
        END TYPE PEOPLE
        TYPE(PEOPLE) :: SMITH = PEOPLE(25,'John Smith')
      END
