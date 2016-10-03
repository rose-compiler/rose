! I think this is using F03 syntax for procedure pointers.
! So this might really be a FFortran 2003 test code 
! (syntax fails for gfortran, but is part of the gfortran test suite).

module modproc29
  implicit none
  interface bar
    procedure x
  end interface bar
  procedure(sub) :: x
  interface
    integer function sub()
    end function sub
  end interface
end module modproc29

integer function x()
  implicit none
  x = -5
end function x

program test
  use modproc29
  implicit none
  integer i
  i = x()
  if(x() /= -5) call abort()
end program test

