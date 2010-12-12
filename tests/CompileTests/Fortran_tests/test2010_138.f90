!   include 'mpif.h'

 ! DQ (12/10/2010): This should be a variable reference, not a function call.
 ! t1wc=MPI_WTIME()

 !  dtwc=t1wc-t0wc
 !  t0wc=t1wc
end

!=========================================
subroutine timer(t0,dt,t0wc,dtwc)
!=========================================
  use precision
  implicit none
!  real(doubleprec) t0,dt,t0wc,dtwc
   real t0,dt,t0wc,dtwc
!  real(doubleprec) t1,t1wc
   real t1,t1wc

! Get cpu usage time since the beginning of the run and subtract value
! from the previous call
!  call cpu_time(t1)
!  dt=t1-t0
!  t0=t1

! Get wall clock time and subtract value from the previous call

! DQ (12/10/2010): Commented out to for the moment.
  t1wc=MPI_WTIME()

!  dtwc=t1wc-t0wc
!  t0wc=t1wc

end subroutine timer

