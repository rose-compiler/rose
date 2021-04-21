program main

  integer :: aa(3)
  integer :: bb(4)
  integer :: cc(5)
  integer :: dd(6)
  common/common1/aa, bb/common2/cc, dd

  print *, aa
  print *, bb
  print *, cc

end program
