subroutine simulation(alpha, beta, gamma, delta, arrays) bind(c)
  use, intrinsic :: iso_c_binding
  implicit none
  integer(c_long), value :: alpha
  real (c_double), intent(inout) :: beta
  integer(c_long), intent(out) :: gamma
  real(c_double),dimension(*),intent(in) :: delta
  type, bind(c) :: pass
     integer(c_int) :: lenc, lenf
     type(c_ptr) :: c, f
  end type pass
  type(pass), intent(inout) :: arrays
  real (c_float), allocatable, target, save :: eta(:)
  real(c_float), pointer :: c_array(:)

  ! Associate c_array with an array allocated in C
  call c_f_pointer (arrays%c, c_array, (/arrays%lenc/) )

  ! Allocate an array and make it available in C
  arrays%lenf = 100
  allocate (eta(arrays%lenf))
  arrays%f = c_loc(eta)

end subroutine simulation

