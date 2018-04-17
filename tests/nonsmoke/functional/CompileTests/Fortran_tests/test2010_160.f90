! S06-io-implied-do-with-stride.f90
! An io-implied-do containing a stride expression causes the front end
! to fail an assertion. An io-implied-do with no stride works correctly.

program p
! write(9,9) (c(i), i=1,9)    ! ok
  write(9,9) (c(i), i=1,7,2)  ! assertion failure: 'objectList' is null

! DQ (1/31/2010): Modified test code to be legal Fortran (with format statement).
9 format (' Required Format Statement ')
end program
