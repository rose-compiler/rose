IMPLICIT NONE
INTEGER, PARAMETER :: n = 10
INTEGER :: i
!$rose scalarization
INTEGER :: a1, a2, a3
INTEGER :: b1(n), b2(n)
INTEGER, DIMENSION(n) :: c
DO i = 1, n
b1(i) = i + 3
b2(i) = i - 2
END DO
!$acc parallel loop gang vector
DO i = 1, 10
a1 = b1(i)
a2 = b1(i) + b2(i)
a3 = b1(i) - 2 * b2(i)
c(i) = 2 * a1 + 3 * a2 + a3 + b1(i)
END DO
END 
