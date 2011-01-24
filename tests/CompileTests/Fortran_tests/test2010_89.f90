! Bug report from Rice: 03-allocatable-in-derived-type.f90
! 'allocatable' attribute in component of derived type is not implemented.

program p
  type :: t
    integer, allocatable :: k  ! produces error message & subsequent assertion failure
  end type t
end program
