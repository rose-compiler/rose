! Bug report from Rice: 24-common-block.f90
! A common block object with an explicit shape specification list
! are not implemented.

program p
  integer :: i(8)
  common i(8)     ! assert false
end program
