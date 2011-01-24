module rngdef
  implicit none
  type :: rng_state
     integer :: indexXXX
     real, dimension(7:10) :: arrayXXX
  end type rng_state
end module rngdef

  use rngdef
  implicit none
  real :: x
  type(rng_state) :: state

  integer n
  real, dimension(7:10) :: arrayXXX
! Unparsed as: "x = state%(array(state%index))" but only if this is a module type.
! The problem is that this is interpreted as a function call.

  x = state%arrayXXX(state%indexXXX)
  x = state%arrayXXX(8)
  x = state%arrayXXX(n)
  x = arrayXXX(state%indexXXX)

end
