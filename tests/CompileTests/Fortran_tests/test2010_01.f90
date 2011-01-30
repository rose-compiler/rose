module precision_01
! use mpi
  include 'original_mpif.h'
! DQ (4/7/2010): removed problem code for OFP (version 0.7.2)
  integer, parameter :: doubleprec=selected_real_kind(12),&
       singleprec=selected_real_kind(6) !,&
!       defaultprec=kind(0.0)
  integer, parameter :: wp=doubleprec,mpi_Rsize=MPI_DOUBLE_PRECISION,&
                       mpi_Csize=MPI_DOUBLE_COMPLEX

  real(wp), parameter :: ONE=1.0d+00
  real(wp), parameter :: TEN=10.0d+00
  real(wp), parameter :: ZERO=0.0d+00
  real(wp), parameter :: PT_ONE=0.1d+00
  real(wp), parameter :: ONE_TEN_THOUSANDTH=1.0d-04
  real(wp), parameter :: SMALL=1.0d-20
  real(wp), parameter :: BIG=1.0d+20

end module precision_01
