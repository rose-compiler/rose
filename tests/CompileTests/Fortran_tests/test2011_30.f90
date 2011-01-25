
module modproc
  implicit none
  interface bar
    procedure x
  end interface bar
  procedure(sub) :: x
  interface
    integer function sub()
    end function sub
  end interface
end module modproc

integer function x()
   implicit none
   x = -5
end function x

program test
  use modproc
  implicit none
  integer i
  i = x()
  if(x() /= -5) call abort()
end
