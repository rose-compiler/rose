program main
  implicit none
  integer :: ii, jj

  ii = 13

  if (ii < 13) then
     jj = ii
     ii = 2
  else
     jj = 1
     ii = 3
  end if

end program
