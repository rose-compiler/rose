! In ROSE 882, in FortranParserAction.C, in 'c_action_return_stmt',
! the occurrence of an integer expression in the return statement is
! not implemented. Worse, the expression is not even popped off the
! astExpressionStack and so can subsequently cause ROSE to fail an assert
! on unrelated valid Fortran such as the 'real' declaration below.


subroutine f(z)
  integer :: z
  return z
end subroutine

subroutine g(x)
  real :: x
end subroutine
