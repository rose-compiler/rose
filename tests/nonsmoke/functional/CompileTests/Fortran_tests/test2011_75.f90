! multiple-char-entity-decl.f90
! In a character type declaration containing two or more variables with different attributes,
! the attributes of all non-array variables following the first variable are lost when unparsed.

program p
  character :: g, h * 9                ! =>  character :: g, h
  character :: d*5, e*(6)              ! =>  character(len=5) :: d,e
  character :: x*4, y(7), z*(8), w(10) ! =>  character(len=4) :: x,y(7),z,w(10)
end program
