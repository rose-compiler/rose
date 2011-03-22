module mpi
   EXTERNAL MPI_WTIME
   REAL*8 MPI_WTIME
end module

program mpi_test
   use mpi
   real x
!  EXTERNAL MPI_WTIME
!  REAL*8 MPI_WTIME
   x = MPI_WTIME()
end
