SUBROUTINE format_runtime (fmtstr)
  IMPLICIT NONE
  CHARACTER(len=*) :: fmtstr
  CHARACTER(len=32), PARAMETER :: str = "hello"

  PRINT fmtstr, str, str, str
END SUBROUTINE format_runtime

PROGRAM main
  IMPLICIT NONE
  CALL format_runtime ('(A, Q, A)')
END PROGRAM main
