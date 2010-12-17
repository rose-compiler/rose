module rngdef
  ! basic definitions
  implicit none
  integer, parameter :: double=selected_real_kind(12)
  integer, parameter :: single=kind(0.0)
  integer, parameter :: rng_k=100, rng_s=8, rng_c=34
  type :: rng_state
     integer :: index
     real(kind=double), dimension(0:rng_k-1) :: array
  end type rng_state
end module rngdef



module rngf77
  ! interface for the original f77 routines
  use rngdef
  implicit none
  interface
     function random(ri,ra)
       use rngdef
       implicit none
       real(kind=double) :: random
       integer, intent(inout) :: ri
       real(kind=double), intent(inout), dimension(0:rng_k-1) :: ra
     end function random

     function srandom(ri,ra)
       use rngdef
       implicit none
       real(kind=single) :: srandom
       integer, intent(inout) :: ri
       real(kind=double), intent(inout), dimension(0:rng_k-1) :: ra
     end function srandom

     subroutine random_array(y,n,ri,ra)
       use rngdef
       implicit none
       integer, intent(in) :: n
       real(kind=double), dimension(0:n-1), intent(out) :: y
       integer, intent(inout) :: ri
       real(kind=double), intent(inout), dimension(0:rng_k-1) :: ra
     end subroutine random_array

     subroutine srandom_array(y,n,ri,ra)
       use rngdef
       implicit none
       integer, intent(in) :: n
       real(kind=single), dimension(0:n-1), intent(out) :: y
       integer, intent(inout) :: ri
       real(kind=double), intent(inout), dimension(0:rng_k-1) :: ra
     end subroutine srandom_array

     subroutine rand_batch(ri,ra)
       use rngdef
       implicit none
       integer, intent(inout) :: ri
       real(kind=double), dimension(0:rng_k-1), intent(inout) :: ra
     end subroutine rand_batch
     subroutine random_init(seed,ri,ra)
       use rngdef
       implicit none
       integer, dimension(0:rng_s-1), intent(in) :: seed
       integer, intent(out) :: ri
       real(kind=double), dimension(0:rng_k-1), intent(out) :: ra
     end subroutine random_init
     subroutine decimal_to_seed(decimal,seed)
       use rngdef
       implicit none
       character(len=*), intent(in) :: decimal
       integer, dimension(0:rng_s-1), intent(out) :: seed
     end subroutine decimal_to_seed
     subroutine string_to_seed(string,seed)
       use rngdef
       implicit none
       character(len=*), intent(in) :: string
       integer, dimension(0:rng_s-1), intent(out) :: seed
     end subroutine string_to_seed
     subroutine set_random_seed(time,seed)
       use rngdef
       implicit none
       integer, dimension(8), intent(in) :: time(8)
       integer, dimension(0:rng_s-1), intent(out) :: seed
     end subroutine set_random_seed
     subroutine seed_to_decimal(seed,decimal)
       use rngdef
       implicit none
       integer, dimension(0:rng_s-1), intent(in) :: seed
       character(len=*), intent(out) :: decimal
     end subroutine seed_to_decimal
     subroutine next_seed3(n0,n1,n2,seed)
       use rngdef
       implicit none
       integer, intent(in) :: n0,n1,n2
       integer, dimension(0:rng_s-1), intent(inout) :: seed
     end subroutine next_seed3
     subroutine next_seed(n0,seed)
       use rngdef
       implicit none
       integer, intent(in) :: n0
       integer, dimension(0:rng_s-1), intent(inout) :: seed
     end subroutine next_seed
  end interface
end module rngf77


subroutine rng_number_d0(state,x)
  use rngf77
  implicit none
  real(kind=double), intent(out) :: x
  type(rng_state), intent(inout) :: state
  real(kind=double), parameter :: ulp2=2.0_double**(-47-1)
  if(state%index.ge.rng_k)then
     call rand_batch(state%index,state%array)
  end if
!  x=state%array(state%index)+ulp2
  state%index=state%index+1
  return
end subroutine rng_number_d0
