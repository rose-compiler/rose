! This test code to demonstrate that the function declaration following a "contains" statement are significant.
! It works with gfortran, but fails with OFP.

    PROGRAM  Cubes
       IMPLICIT   NONE

  ! contains statement without an associated function causes parser to fail
    CONTAINS

    END PROGRAM  Cubes

