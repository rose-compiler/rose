!module rngdef
  implicit none
  type :: rng_state
     integer :: index
     real, dimension(7:10) :: array
  end type rng_state
!end module rngdef

!  use rngdef
  real :: x
  type(rng_state) :: state

! Unparsed as: "x = state%(array(state%index))" but only if this is a module type.
!  x = state%array(state%index)

end
