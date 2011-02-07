program arrays3
  real :: x(2,3), y(2)
  DATA ((x (i,j), i=1,2), j=1,3) / 6 * 2.0 /
  y = x(1:2,2)
  if (y(1).eq. 2.0) then 
    print *, 'OK'
  else
    print *, y
  endif 
end program
