! ROSE correctly parses a pointer to an array if the pointer attribute precedes the dimension attribute.
! 
PROGRAM mod6_5

  REAL, DIMENSION(10), TARGET :: X
  REAL, POINTER, DIMENSION(:) :: Y 
  
  !!! Aside: there is a dangling pointer that is kept on the astBaseTypeStack (or astTypeStack?) in the code.
  
  Y=>X

END PROGRAM
