! Rice test
! A one-element array constructor is unparsed incorrectly.

program singleton_array_constructor
  integer, dimension(1) :: x
  x = (/ 1 /)  ! =>  x = 1
end program
