! If an 'inquire' statement with an inquire-spec-list occurred as the
! controlled statement of an if-then statement, the front end would fail
! an assertion.

program p
  integer :: k = 0
  logical :: exists
  if( k == 0 ) inquire( file = 'somefile', exist = exists )
end program
