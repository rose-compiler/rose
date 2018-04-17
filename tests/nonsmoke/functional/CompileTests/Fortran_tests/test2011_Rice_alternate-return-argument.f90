! Rice test
! An alternate-return dummy argument in a subroutine statement
! causes the front end to fail an assertion in 'generateImplicitType'.

subroutine g(*, a, b, *, *, i, *)  ! assertion failure
  return 1
  return 3
  return 2
  return 4
end subroutine
