! Bug report from Rice: 22-endfile.f90
! An 'endfile' statement causes the front end to fail an assertion.

program p
  endfile 10  ! assertion failure: name stack not empty
end program
