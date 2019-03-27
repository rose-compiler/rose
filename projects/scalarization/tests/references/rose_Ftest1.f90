SUBROUTINE test()
INTEGER, PARAMETER :: vecsize = 10
!$rose scalarization
REAL(kind=8) :: array1
DO i = 1, vecsize
array1 = 1.
END DO
END SUBROUTINE test

