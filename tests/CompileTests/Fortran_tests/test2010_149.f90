

module rngdef149
  implicit none
  integer, parameter :: double=selected_real_kind(12)
  integer, parameter :: rng_k=100, rng_s=8, rng_c=34
  type :: rng_state
     integer :: index
   ! Error: unparsed as "REAL(kind=double), DIMENSION(rng_k - 1) :: array = (/0/)"
     real(kind=double), dimension(0:rng_k-1) :: array
   ! real, dimension(7:42) :: array
  end type rng_state
end module rngdef149


module rng_gtc
  use rngdef149
  type(rng_state) :: state
end module rng_gtc


subroutine rand_num_gen_init
  use rng_gtc
  implicit none
  real, dimension(7:9) :: array
end
