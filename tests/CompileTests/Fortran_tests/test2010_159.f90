! S05-data-implied-do.f90
! Any data-implied-do causes the front end to fail an assertion.
! This is actually a not-implemented feature in 'c_action_data_implied_do'
! at FortranParserActionROSE.C:5341 (in ROSE 903).

program p
  integer :: k
  data (a(k), k=1,3) / 101, 102, 103/  ! assertion failure: doLoopVar is null
end program
