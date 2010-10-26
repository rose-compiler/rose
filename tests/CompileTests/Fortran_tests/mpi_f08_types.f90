! Copyright (c) 2009 Cisco Systems, Inc.  All rights reserved.
!
! This file creates mappings between MPI C types (e.g., MPI_Comm) and
! variables (e.g., MPI_COMM_WORLD) and corresponding Fortran names
! (type(MPI_Comm_world) and MPI_COMM_WORLD, respectively).

module mpi_f08_types

   use, intrinsic :: ISO_C_BINDING

   !
   ! constants (these must agree with those in mpif-common.h, mpif-config.h)
   !
   integer(C_INT), parameter :: MPI_STATUS_SIZE = 5

   !
   ! kind parameters
   !

   integer(C_INT), parameter :: MPI_COUNT_KIND   = C_INT
   integer(C_INT), parameter :: MPI_ADDRESS_KIND = C_SIZE_T
   integer(C_INT), parameter :: MPI_DOUBLE_KIND  = C_DOUBLE
   integer(C_INT), parameter :: MPI_OFFSET_KIND  = C_SIZE_T  !TODO - is this correct (CER)

   !
   ! derived types
   !

   type, BIND(C) :: MPI_Comm
      integer(C_INT) :: MPI_VAL
   end type MPI_Comm

   type, BIND(C) :: MPI_Datatype
      integer(C_INT) :: MPI_VAL
   end type MPI_Datatype

   type, BIND(C) :: MPI_Errhandler
      integer(C_INT) :: MPI_VAL
   end type MPI_Errhandler

   type, BIND(C) :: MPI_File
      integer(C_INT) :: MPI_VAL
   end type MPI_File

   type, BIND(C) :: MPI_Group
      integer(C_INT) :: MPI_VAL
   end type MPI_Group

   type, BIND(C) :: MPI_Info
      integer(C_INT) :: MPI_VAL
   end type MPI_Info

   type, BIND(C) :: MPI_Op
      integer(C_INT) :: MPI_VAL
   end type MPI_Op

   type, BIND(C) :: MPI_Request
      integer(C_INT) :: MPI_VAL
   end type MPI_Request

   type, BIND(C) :: MPI_Win
      integer(C_INT) :: MPI_VAL
   end type MPI_Win

   type, BIND(C) :: MPI_Status
      integer(C_INT) :: MPI_SOURCE
      integer(C_INT) :: MPI_TAG
      integer(C_INT) :: MPI_ERROR
      ! JMS, Can we make just these last 2 members be private?
      ! CER, No
      integer(C_INT) :: c_count
      integer(C_INT) :: c_cancelled
   end type MPI_Status

  !
  ! Typedefs from C
  !

! MPI_Aint
! MPI_Offset

  !
  ! Pre-defined communicator bindings
  !

  type(MPI_Comm), protected, bind(C, name="ompi_f08_mpi_comm_world") :: MPI_COMM_WORLD
! type(MPI_Comm), protected, bind(C) :: MPI_COMM_WORLD

  type(MPI_Comm), protected, bind(C, name="ompi_f08_mpi_comm_self")  :: MPI_COMM_SELF
  type(MPI_Group),      protected, bind(C, name="ompi_f08_mpi_group_empty")      :: MPI_GROUP_EMPTY
  type(MPI_Errhandler), protected, bind(C, name="ompi_f08_mpi_errors_are_fatal") :: MPI_ERRORS_ARE_FATAL
  type(MPI_Errhandler), protected, bind(C, name="ompi_f08_mpi_errors_return")    :: MPI_ERRORS_RETURN

  !
  !  NULL "handles" (indices)
  !

  type(MPI_Group),      protected, bind(C, name="ompi_f08_mpi_group_null")      :: MPI_GROUP_NULL;
  type(MPI_Comm),       protected, bind(C, name="ompi_f08_mpi_comm_null")       :: MPI_COMM_NULL;
  type(MPI_Datatype),   protected, bind(C, name="ompi_f08_mpi_datatype_null")   :: MPI_DATATYPE_NULL;
  type(MPI_Request),    protected, bind(C, name="ompi_f08_mpi_request_null")    :: MPI_REQUEST_NULL;
  type(MPI_Op),         protected, bind(C, name="ompi_f08_mpi_op_null")         :: MPI_OP_NULL;
  type(MPI_Errhandler), protected, bind(C, name="ompi_f08_mpi_errhandler_null") :: MPI_ERRHANDLER_NULL;
  type(MPI_Info),       protected, bind(C, name="ompi_f08_mpi_info_null")       :: MPI_INFO_NULL;
  type(MPI_Win),        protected, bind(C, name="ompi_f08_mpi_win_null")        :: MPI_WIN_NULL;

  !
  ! Pre-defined datatype bindings
  !
  !   These definitions should match those in ompi/include/mpif-common.h.
  !   They are defined in ompi/runtime/ompi_mpi_init.c
  !

  type(MPI_Datatype), protected, bind(C, name="ompi_f08_mpi_byte")              :: MPI_BYTE
  type(MPI_Datatype), protected, bind(C, name="ompi_f08_mpi_packed")            :: MPI_PACKED
  type(MPI_Datatype), protected, bind(C, name="ompi_f08_mpi_ub")                :: MPI_UB
  type(MPI_Datatype), protected, bind(C, name="ompi_f08_mpi_lb")                :: MPI_LB
  type(MPI_Datatype), protected, bind(C, name="ompi_f08_mpi_character")         :: MPI_CHARACTER
  type(MPI_Datatype), protected, bind(C, name="ompi_f08_mpi_logical")           :: MPI_LOGICAL
  type(MPI_Datatype), protected, bind(C, name="ompi_f08_mpi_integer")           :: MPI_INTEGER
  type(MPI_Datatype), protected, bind(C, name="ompi_f08_mpi_integer1")          :: MPI_INTEGER1
  type(MPI_Datatype), protected, bind(C, name="ompi_f08_mpi_integer2")          :: MPI_INTEGER2
  type(MPI_Datatype), protected, bind(C, name="ompi_f08_mpi_integer4")          :: MPI_INTEGER4
  type(MPI_Datatype), protected, bind(C, name="ompi_f08_mpi_integer8")          :: MPI_INTEGER8
  type(MPI_Datatype), protected, bind(C, name="ompi_f08_mpi_integer16")         :: MPI_INTEGER16
  type(MPI_Datatype), protected, bind(C, name="ompi_f08_mpi_real")              :: MPI_REAL
  type(MPI_Datatype), protected, bind(C, name="ompi_f08_mpi_real4")             :: MPI_REAL4
  type(MPI_Datatype), protected, bind(C, name="ompi_f08_mpi_real8")             :: MPI_REAL8
  type(MPI_Datatype), protected, bind(C, name="ompi_f08_mpi_real16")            :: MPI_REAL16
  type(MPI_Datatype), protected, bind(C, name="ompi_f08_mpi_double_precision")  :: MPI_DOUBLE_PRECISION
  type(MPI_Datatype), protected, bind(C, name="ompi_f08_mpi_complex")           :: MPI_COMPLEX
  type(MPI_Datatype), protected, bind(C, name="ompi_f08_mpi_complex8")          :: MPI_COMPLEX8
  type(MPI_Datatype), protected, bind(C, name="ompi_f08_mpi_complex16")         :: MPI_COMPLEX16
  type(MPI_Datatype), protected, bind(C, name="ompi_f08_mpi_complex32")         :: MPI_COMPLEX32
  type(MPI_Datatype), protected, bind(C, name="ompi_f08_mpi_double_complex")    :: MPI_DOUBLE_COMPLEX
  type(MPI_Datatype), protected, bind(C, name="ompi_f08_mpi_2real")             :: MPI_2REAL
  type(MPI_Datatype), protected, bind(C, name="ompi_f08_mpi_2double_precision") :: MPI_2DOUBLE_PRECISION
  type(MPI_Datatype), protected, bind(C, name="ompi_f08_mpi_2integer")          :: MPI_2INTEGER
  type(MPI_Datatype), protected, bind(C, name="ompi_f08_mpi_2complex")          :: MPI_2COMPLEX
  type(MPI_Datatype), protected, bind(C, name="ompi_f08_mpi_2double_complex")   :: MPI_2DOUBLE_COMPLEX
  type(MPI_Datatype), protected, bind(C, name="ompi_f08_mpi_real2")             :: MPI_REAL2
  type(MPI_Datatype), protected, bind(C, name="ompi_f08_mpi_logical1")          :: MPI_LOGICAL1
  type(MPI_Datatype), protected, bind(C, name="ompi_f08_mpi_logical2")          :: MPI_LOGICAL2
  type(MPI_Datatype), protected, bind(C, name="ompi_f08_mpi_logical4")          :: MPI_LOGICAL4
  type(MPI_Datatype), protected, bind(C, name="ompi_f08_mpi_logical8")          :: MPI_LOGICAL8

! Some random MPI constants
  integer, parameter :: MPI_MAX_OBJECT_NAME = 64 - 1
  integer, parameter :: MPI_MAX_PROCESSOR_NAME = 256 - 1

!
! The following types and procedures are here temporarily,
! for testing purposes only
!

integer, parameter :: INTPTR_T_KIND = C_INTPTR_T
integer, parameter :: CFI_MAX_RANK = 7  ! until F2008 compilers

type, bind(C) :: CFI_dim_t
   integer(INTPTR_T_KIND) :: lower_bound, extent, sm;
end type CFI_dim_t

type, bind(C) :: CFI_cdesc_t
   type(C_PTR)       :: base_addr    ! base address of object
   integer(C_SIZE_T) :: elem_len     ! length of one element, in bytes
   integer(C_INT)    :: rank         ! object rank, 0 .. CF_MAX_RANK
   integer(C_INT)    :: type         ! identifier for type of object
   integer(C_INT)    :: attribute    ! object attribute: 0..2, or -1
   integer(C_INT)    :: state        ! allocation/association state: 0 or 1

!  type(CFI_dim_t)   :: dim(CFI_MAX_RANK)  ! dimension triples
   TYPE ( CFI_dim_t ) , DIMENSION(CFI_MAX_RANK) :: dim

end type CFI_cdesc_t

end module mpi_f08_types
