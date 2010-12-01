! ROSE incorrectly parses a pointer to an array if the pointer attribute follows the dimension attribute.
!
PROGRAM prog6_4

  REAL, DIMENSION(10), TARGET :: X
  ! pointer attribute will be lost
  REAL, DIMENSION(:), POINTER :: Y 
  
  Y => X

END
