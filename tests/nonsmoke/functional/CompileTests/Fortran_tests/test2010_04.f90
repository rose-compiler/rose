
program gtc
!  use global_parameters
!  use particle_array
!  use particle_tracking
!  use field_array
!  use diagnosis_array
  implicit none

  integer, parameter :: doubleprec=selected_real_kind(12),&
       singleprec=selected_real_kind(6) !,&
!       defaultprec=kind(0.0)
  integer, parameter :: wp=doubleprec !,mpi_Rsize=MPI_DOUBLE_PRECISION, mpi_Csize=MPI_DOUBLE_COMPLEX

  REAL(kind=wp) :: nonlinear

!  integer i,ierror
!  real(doubleprec) time(9),timewc(9),t0,dt,t0wc,dtwc,loop_time
!  real(doubleprec) tracktcpu,tracktwc,tr0,tr0wc
!  character(len=10) ic(8)

end program gtc
