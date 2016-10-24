! In a character type declaration containing two or more variables with different attributes,
! the attributes of all non-array variables following the first variable are lost.
program p
  character :: g, h * 9      ! becomes character :: g, h
  character :: d*5, e*(6)    ! becomes character(len=5) :: d,e
  character :: a(2), b(3)    ! OK
  character :: x*4, y(7), z*(8), w(10) ! becomes character(len=4) :: x,y(7),z,w(10)
  character(len = 2) ::  s, c(3)*(4)  = 'abcd'
  s = 'xyz'
  print *, s
  print *, c(1)
  print *, c(2)
end program
