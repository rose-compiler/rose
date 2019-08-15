SUBROUTINE test()
INTEGER, PARAMETER :: vecsize = 10
REAL(kind=8), DIMENSION(vecsize) :: array1
!$rose scalarization
REAL(kind=8) :: array2
DO i = 1, vecsize
array2 = array1(i)
END DO
END SUBROUTINE test

