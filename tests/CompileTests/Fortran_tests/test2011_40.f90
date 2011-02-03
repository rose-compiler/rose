! Contributed by Jean Utke to the bug tracker (1/31/2011)
! This code demonstrates a problem with the INQUIRE statement.

program p 
  implicit none 
  integer u, i 
  logical alreadyInUse
  do i=1,10
    ! parser test for inquire
    inquire(unit=u, opened=alreadyInUse)
  end do 
  print *,'OK'
end program 
