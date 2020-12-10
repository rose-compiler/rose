SUBROUTINE test()
INTEGER, PARAMETER :: vecsize = 10
!$rose scalarization(2)
REAL(kind=8), DIMENSION(5,10) :: array1
DO k = 1, 10
DO i = 1, vecsize
DO j = 1, 5
array1(i,k) = 1.
END DO
END DO
END DO
END SUBROUTINE test

