! Rice test
! Any data-implied-do causes the front end to fail an assertion.
! This is actually a not-implemented feature in 'c_action_data_implied_do'
! at FortranParserActionROSE.C:5341 (in ROSE 903).

program data_implied_do
  integer :: a(3)
  data (a(k), k=1,3, 1) / 101, 102, 103/  ! assertion failure: doLoopVar is null
end program
