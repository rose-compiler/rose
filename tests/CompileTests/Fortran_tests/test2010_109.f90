! Bug report from Rice: 23-backspace.f90
! A 'backspace' statement causes the front end to fail an assertion.

program p
  backspace 10  ! assertion failure: name stack not empty
end program
