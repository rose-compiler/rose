! vector constructor with implied do loop is mistranslated

program f
  integer :: v(4)
  v = (/ (i, i=1,4, 1) /)  ! mistranslated as v = (i, i=1,4, 1) 
end program f
