! This is a small test program used by Laksono Adhianto at Rice
    ! -----------------------------------------------------
    !    This program display the cubes of INTEGERs and 
    ! REALs.  The cubes are computed with two functions:
    ! intCube() and realCube().
    ! -----------------------------------------------------

    PROGRAM  Cubes
       IMPLICIT   NONE

       INTEGER, PARAMETER :: Iterations = 10
       INTEGER            :: i
       REAL               :: x

       DO i = 1, Iterations
          x = i
          WRITE(*,*)  i, x, intCube(i), realCube(x)
       END DO

    CONTAINS

    ! -----------------------------------------------------
    ! INTEGER FUNCTION intCube() :
    !    This function returns the cube of the argument.
    ! -----------------------------------------------------

       INTEGER  FUNCTION  intCube(Number)
          IMPLICIT   NONE

          INTEGER, INTENT(IN) :: Number

          intCube = Number*Number*Number
       END FUNCTION  intCube

    ! -----------------------------------------------------
    ! REAL FUNCTION realCube() :
    !    This function returns the cube of the argument.
    ! -----------------------------------------------------

       REAL FUNCTION  realCube(Number)
          IMPLICIT  NONE

          REAL, INTENT(IN) :: Number

          realCube = Number*Number*Number
       END FUNCTION  realCube

    END PROGRAM  Cubes

