!This is a simple test program
!written to do I donot know what
PROGRAM Triangle_Area

  IMPLICIT NONE

  TYPE triangle
     REAL :: a, b, c
  END TYPE triangle

  TYPE(triangle) :: t

  PRINT*, 'Welcome, please enter the lengths of the 3 sides.'
  READ*, t%a, t%b, t%c
  PRINT*,'Triangle''s area: ',Area(t)

CONTAINS

  FUNCTION Area(tri)

    IMPLICIT NONE

    REAL :: Area       ! function type
    TYPE(triangle), INTENT( IN ) :: tri
    REAL :: theta, height
    ! Is this comment recognized???

    theta  = ACOS( (tri%a**2 + tri%b**2 - tri%c**2) / (2.0*tri%a*tri%b) )
    height = tri%a*SIN(theta); Area = 0.5*tri%b*height

  END FUNCTION Area

END PROGRAM Triangle_Area
!This ends the program


