! Bug report from Rice: 08-string-literal-substring.f90
! Taking a subrange of a string constant causes the front end
! to fail an assertion.

program p
  character(10) :: c
  c = 'ab'(1:1)    ! fail: astExpressionStack not empty
  c = c(1:1)       ! ok
end program
