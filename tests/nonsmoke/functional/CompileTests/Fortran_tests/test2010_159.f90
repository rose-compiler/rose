! S05-data-implied-do.f90
! Any data-implied-do causes the front end to fail an assertion.
! This is actually a not-implemented feature in 'c_action_data_implied_do'
! at FortranParserActionROSE.C:5341 (in ROSE 903).

program p
   integer :: k,i

 ! DQ (1/31/2010): Added declaration for "a" to make this a legal Fortran code (else this code fails in syntax checking).
   integer a(3)

   data (a(k), k=1,3) / 101, 102, 103/  ! assertion failure: doLoopVar is null
end program
