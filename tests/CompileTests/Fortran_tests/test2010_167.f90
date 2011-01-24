! S11-function-where-var-expected.f90
! ROSE gives misleading output when a function name is used
! where a variable name is expected.
!
! Here 'idim' is a Fortran intrinsic function.

program p
  do idim = 1,10  ! assertion failure: 'variableSymbol' is null
  end do
end program
