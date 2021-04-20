program main

  integer :: aa(3)
  integer :: bb(4)
  integer :: cc(5)
  common/common1/aa, bb, cc

  print *, aa
  print *, bb
  print *, cc

end program
