module mpi_f08_types_10_75

   use, intrinsic :: ISO_C_BINDING

   integer(C_INT), parameter :: MPI_STATUS_SIZE = 5

 ! type, BIND(C) :: MPI_Comm
   type, PUBLIC, BIND(C) :: MPI_Comm
      integer(C_INT) :: MPI_VAL
   end type MPI_Comm

 ! This is the problem statement.
   type(MPI_Comm), protected, bind(C, name="ompi_f08_mpi_comm_world") :: MPI_COMM_WORLD
 ! type(MPI_Comm), protected, bind(C) :: MPI_COMM_WORLD
 ! type(MPI_Comm), public, bind(C, name="ompi_f08_mpi_comm_world") :: MPI_COMM_WORLD

end module mpi_f08_types_10_75
