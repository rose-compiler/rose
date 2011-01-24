! Bug report from Rice: 13-generic-binding.f90
! A generic binding in a derived type causes an assertion failure
! in the front end.

program p
  type :: t
  contains
    generic :: operator(+) => plus  ! assertion failure: 'name stack not empty'
  end type 
end program
