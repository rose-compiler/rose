PROGRAM Triangle_Area

  TYPE triangle
     sequence
! This is an error fro gfortran 4.2, but not for 4.1.2
!    private
     integer :: a
     REAL :: b
  END TYPE triangle

  TYPE(triangle) :: t

END PROGRAM Triangle_Area
!This ends the program
