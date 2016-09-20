! Bug report from Jean (ANL) modVar.f90
module a
  implicit none
  private 
  save
  integer, parameter, public :: km =3
end module 

module b  
  use a
    
  real, dimension(km), public :: zw 
end module

module c
  use b
contains
subroutine foo(x)
  real :: x
  real :: zw
  zw=x 
  zw=zw+1.0
  x=zw
end subroutine
end module 
 
program p 
 use c
 integer k
 k=1
 call foo(zw(k))
 print *,'OK' ! parser test only
end program
