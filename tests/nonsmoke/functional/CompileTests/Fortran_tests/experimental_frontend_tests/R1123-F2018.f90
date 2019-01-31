program main

  integer :: i, j
  real :: a, b

  do concurrent(i=1:10:2, j=3:13)
     a = 1
     b = a
  end do

end program
