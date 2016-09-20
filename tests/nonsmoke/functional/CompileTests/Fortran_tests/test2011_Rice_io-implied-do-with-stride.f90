! Rice test
! An io-implied-do containing a stride expression causes the front end
! to fail an assertion. An io-implied-do with no stride works correctly.

program io_implied_do_with_stride
  integer :: c(20)
!  write(9,*) (c(i), i=1,9)    ! ok
  write(9,*) (c(i), i=1,9, 2)  ! assertion failure: 'objectList' is null
!  print *, (c(i), i = 1,9,2)  
end program
