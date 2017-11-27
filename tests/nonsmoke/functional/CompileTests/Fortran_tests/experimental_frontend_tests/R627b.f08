!! R627 alloc-opt
!    or SOURCE = source-expr
!   - F2008 addition
!   - SOURCE= in ALLOCATE statement can give array bounds as well as the value

program alloc_opt
  real :: old(2,4)
  real, allocatable :: new(:,:)

  old(1,1:4) = [11,12,13,14]
  old(2,1:4) = [21,22,23,24]

  ALLOCATE (new, SOURCE=old(1:2,1:3))

end program
