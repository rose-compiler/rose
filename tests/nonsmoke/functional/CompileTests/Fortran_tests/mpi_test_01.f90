program mpi_example
  use mpi

! MPI initialize
  call mpi_init(ierror)
 
! MPI finalize
  call mpi_finalize(ierror)
end


!=========================================
subroutine timer(t0,dt,t0wc,dtwc)
!=========================================
  use mpi
  implicit none

   real t0,dt,t0wc,dtwc
   real time

! Get wall clock time 
  time=MPI_WTIME()

end subroutine timer
