module single
  integer, parameter :: float = selected_real_kind ( 6,37 )
  real ( float ) :: pi = 3.1415927_float
  real ( float ) :: e  = 2.7182818_float
end module single

module double
  integer, parameter :: float = selected_real_kind ( 15,37 )
  real ( float ) :: pi = 3.141592653589793_float
  real ( float ) :: e  = 2.718281828459045_float
end module double

program constants
  !
  !     Select a precision for the constants for PI and E.
  !     USE SINGLE selects single precision whereas
  !     USE DOUBLE selects double precision.  Only one
  !     precision can be in selected per subprogram.
  !     Let the main program select single precision.
  use single
  character (len=60) :: pi_single
  character (len=60) :: e_single
  integer :: eslen, pislen
  write (UNIT=pi_single, FMT=*) pi
  write (UNIT=e_single, FMT=*) e
  pislen=len_trim(adjustl(pi_single))
  eslen=len_trim(adjustl(e_single))
  if((pislen.GE.7).AND.(eslen.GE.7)) then
     call more_precision
  else
     print *, "failed"
  end if
end program constants

subroutine more_precision
  !
  !     Let the subroutine select double precision.
  use double
  character (len=60) :: pi_double
  character (len=60) :: e_double
  integer :: edlen, pidlen
  write (UNIT=pi_double, FMT=*) pi
  write (UNIT=e_double, FMT=*) e
  pidlen=len_trim(adjustl(pi_double))
  edlen=len_trim(adjustl(e_double))

  if((pidlen.GE.16).AND.(edlen.GE.16)) then
     print *, "OK"
  else
     print *, "failed"
  end if
end subroutine more_precision
