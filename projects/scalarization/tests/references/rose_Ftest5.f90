SUBROUTINE test()
INTEGER, PARAMETER :: vecsize = 10
!$rose scalarization
REAL(kind=8), DIMENSION(5) :: array1
DO j = 1, 5
DO i = 1, vecsize
array1(j) = 1.
END DO
END DO
END SUBROUTINE test

