SUBROUTINE TEST
integer :: n = 1

ZERO: SELECT CASE(N)

  CASE (0)
    n = 0
  CASE (7)
    n = 42

END SELECT ZERO
END

