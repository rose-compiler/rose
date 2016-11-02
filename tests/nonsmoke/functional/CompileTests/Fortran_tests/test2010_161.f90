! S07-alternate-return-argument.f90
! An alternate-return dummy argument in a subroutine statement
! causes the front end to fail an assertion in 'generateImplicitType'.

subroutine g(*)  ! assertion failure
  return 1
end subroutine
