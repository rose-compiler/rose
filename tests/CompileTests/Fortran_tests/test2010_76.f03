
module mpi_f08_types

   use, intrinsic :: ISO_C_BINDING

   integer(C_INT), parameter :: MPI_STATUS_SIZE = 5

 ! type, BIND(C) :: MPI_Comm
 ! type, PUBLIC, BIND(C) :: MPI_Comm
 !    integer(C_INT) :: MPI_VAL
 ! end type MPI_Comm

 ! This is the problem statement.
 ! type(MPI_Comm), protected, bind(C, name="ompi_f08_mpi_comm_world") :: MPI_COMM_WORLD
 ! type(MPI_Comm), protected, bind(C) :: MPI_COMM_WORLD



   TYPE , PUBLIC, BIND(C) :: CFI_cdesc_t
      INTEGER :: base_addr
      INTEGER(kind=C_SIZE_T) :: elem_len
      INTEGER(kind=C_INT) :: rank
      INTEGER(kind=C_INT) :: type
      INTEGER(kind=C_INT) :: attribute
      INTEGER(kind=C_INT) :: state
    ! This is the problem statement.
      TYPE ( CFI_dim_t ) , DIMENSION(CFI_MAX_RANK) :: dim
   END TYPE CFI_cdesc_t

end module mpi_f08_types
