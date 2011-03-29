! Rice test
! vector constructor with implied do loop is mistranslated

program vector_constructor_1
  integer :: v(4)
  v = (/ 1, 2, 3,  4 /) ! OK
  v = (/ (i, i=1,4) /)  ! mistranslated as v = (i, i=1,4) 
end program
