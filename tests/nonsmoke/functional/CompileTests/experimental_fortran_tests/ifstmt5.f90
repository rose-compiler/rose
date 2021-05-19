program main
  implicit none
  integer :: ii, jj

  ii = 13

  if (ii > 13) then
     jj = ii
     ii = 2
  else if (ii == 2) then
     jj = 123
     ii = 321
  else if (ii == 3) then
     jj = 1234
     ii = 4321
  else if (ii == 4) then
     jj = 11
     ii = 111
  else if (ii == 5) then
     jj = 12345
     ii = 54321
  else
     jj = 1
     ii = 3
  end if

end program
