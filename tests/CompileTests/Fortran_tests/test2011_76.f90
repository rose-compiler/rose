! singleton-array-constructor.f90
! A one-element array constructor is unparsed incorrectly.

program p
  integer, dimension(1) :: x
  x = (/ 1 /)  ! =>  x = 1
end program
