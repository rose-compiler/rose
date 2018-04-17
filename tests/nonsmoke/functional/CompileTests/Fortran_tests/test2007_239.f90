! Example of forall statement
REAL :: A(5,2)
FORALL (I = 1:5,J = 2:4) A(I,J) = 42.0
! WRITE (*,*) A
END
