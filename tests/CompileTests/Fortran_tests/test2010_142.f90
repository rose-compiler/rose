module mpi142
   EXTERNAL MPI_WTIME
   REAL*8 MPI_WTIME
end module

program mpi_test
   use mpi142
   real x
!  EXTERNAL MPI_WTIME
!  REAL*8 MPI_WTIME
   x = MPI_WTIME()
end
