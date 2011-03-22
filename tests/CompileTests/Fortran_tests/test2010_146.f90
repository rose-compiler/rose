module rngdef
  implicit none
  integer, parameter :: double=selected_real_kind(12)
  integer, parameter :: rng_k=100, rng_s=8, rng_c=34
  type :: rng_state
     integer :: index

   ! Error in unparser generates "REAL(kind=double), DIMENSION(rng_k - 1) :: array = (/0/)"
     real(kind=double), dimension(0:rng_k-1) :: array

  end type rng_state
end module rngdef



subroutine rng_number_d0(state,x)
  use rngdef

  implicit none

  real(kind=double), intent(out) :: x
  type(rng_state), intent(inout) :: state

! Unparsed as: "x = state%(array(state%index))"
  x = state%array(state%index)

  return
end subroutine rng_number_d0
