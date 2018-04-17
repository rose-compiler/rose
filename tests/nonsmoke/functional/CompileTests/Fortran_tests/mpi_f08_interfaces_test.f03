interface

SUBROUTINE MPI_Bsend(buf, count, datatype, dest, tag, comm, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: buf
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count, dest, tag 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Bsend_init(buf, count, datatype, dest, tag, comm, request, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count, dest, tag 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Comm), INTENT(IN) :: comm 
TYPE(MPI_Request), INTENT(OUT) :: request 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Buffer_attach(buffer, size, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: buffer 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buffer
!$PRAGMA IGNORE_TKR buffer
REAL, DIMENSION(*) :: buffer
INTEGER, INTENT(IN) :: size 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Buffer_detach(buffer_addr, size, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: buffer_addr 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buffer_addr
!$PRAGMA IGNORE_TKR buffer_addr
REAL, DIMENSION(*) :: buffer_addr
INTEGER, INTENT(OUT) :: size 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Cancel(request, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Request), INTENT(IN) :: request 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Get_count(status, datatype, count, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Status), INTENT(IN) :: status
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
INTEGER, INTENT(OUT) :: count 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Ibsend(buf, count, datatype, dest, tag, comm, request, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count, dest, tag 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Comm), INTENT(IN) :: comm 
TYPE(MPI_Request), INTENT(OUT) :: request 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Iprobe(source, tag, comm, flag, status, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: source, tag 
TYPE(MPI_Comm), INTENT(IN) :: comm 
LOGICAL, INTENT(OUT) :: flag 
TYPE(MPI_Status) :: status
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Irecv(buf, count, datatype, source, tag, comm, request, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count, source, tag 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Comm), INTENT(IN) :: comm 
TYPE(MPI_Request), INTENT(OUT) :: request 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Irsend(buf, count, datatype, dest, tag, comm, request, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count, dest, tag 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Comm), INTENT(IN) :: comm 
TYPE(MPI_Request), INTENT(OUT) :: request 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Isend(buf, count, datatype, dest, tag, comm, request, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count, dest, tag 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Comm), INTENT(IN) :: comm 
TYPE(MPI_Request), INTENT(OUT) :: request 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Issend(buf, count, datatype, dest, tag, comm, request, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count, dest, tag 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Comm), INTENT(IN) :: comm 
TYPE(MPI_Request), INTENT(OUT) :: request 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Probe(source, tag, comm, status, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: source, tag 
TYPE(MPI_Comm), INTENT(IN) :: comm 
TYPE(MPI_Status) :: status
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Recv(buf, count, datatype, source, tag, comm, status, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count, source, tag 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Comm), INTENT(IN) :: comm 
TYPE(MPI_Status) :: status
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Recv_init(buf, count, datatype, source, tag, comm, request, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count, source, tag 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Comm), INTENT(IN) :: comm 
TYPE(MPI_Request), INTENT(OUT) :: request 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Request_free(request, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Request), INTENT(INOUT) :: request 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Request_get_status( request, flag, status, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Request), INTENT(IN) :: request 
LOGICAL, INTENT(OUT) :: flag 
TYPE(MPI_Status) :: status
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Rsend(buf, count, datatype, dest, tag, comm, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count, dest, tag 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Rsend_init(buf, count, datatype, dest, tag, comm, request, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count, dest, tag 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Comm), INTENT(IN) :: comm 
TYPE(MPI_Request), INTENT(OUT) :: request 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Send(buf, count, datatype, dest, tag, comm, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count, dest, tag 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Sendrecv(sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, &
      recvcount, recvtype, source, recvtag, comm, status, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: sendbuf, recvbuf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: sendbuf, recvbuf
!$PRAGMA IGNORE_TKR sendbuf, recvbuf
REAL, DIMENSION(*) :: sendbuf, recvbuf
INTEGER, INTENT(IN) :: sendcount, dest, sendtag, recvcount, source, recvtag 
TYPE(MPI_Datatype), INTENT(IN) :: sendtype 
TYPE(MPI_Datatype), INTENT(IN) :: recvtype 
TYPE(MPI_Comm), INTENT(IN) :: comm 
TYPE(MPI_Status) :: status
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Sendrecv_replace(buf, count, datatype, dest, sendtag, source, recvtag, comm, status, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count, dest, sendtag, source, recvtag 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Comm), INTENT(IN) :: comm 
TYPE(MPI_Status) :: status
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Send_init(buf, count, datatype, dest, tag, comm, request, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count, dest, tag 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Comm), INTENT(IN) :: comm 
TYPE(MPI_Request), INTENT(OUT) :: request 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Ssend(buf, count, datatype, dest, tag, comm, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count, dest, tag 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Ssend_init(buf, count, datatype, dest, tag, comm, request, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count, dest, tag 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Comm), INTENT(IN) :: comm 
TYPE(MPI_Request), INTENT(OUT) :: request 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Start(request, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Request), INTENT(INOUT) :: request 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Startall(count, array_of_requests, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: count 
TYPE(MPI_Request), INTENT(INOUT) :: array_of_requests(*) 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Test(request, flag, status, ierror) 
USE mpi_f08_types
implicit none
LOGICAL, INTENT(OUT) :: flag 
TYPE(MPI_Request), INTENT(INOUT) :: request 
TYPE(MPI_Status) :: status
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Testall(count, array_of_requests, flag, array_of_statuses, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: count 
TYPE(MPI_Request), INTENT(INOUT) :: array_of_requests(*) 
LOGICAL, INTENT(OUT) :: flag 
TYPE(MPI_Status) :: array_of_statuses(*) 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Testany(count, array_of_requests, index, flag, status, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: count 
TYPE(MPI_Request), INTENT(INOUT) :: array_of_requests(*) 
INTEGER, INTENT(OUT) :: index 
LOGICAL, INTENT(OUT) :: flag 
TYPE(MPI_Status) :: status
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Testsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: incount 
TYPE(MPI_Request), INTENT(INOUT) :: array_of_requests(*) 
INTEGER, INTENT(OUT) :: outcount, array_of_indices(*) 
TYPE(MPI_Status) :: array_of_statuses(*) 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Test_cancelled(status, flag, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Status) :: status
LOGICAL, INTENT(OUT) :: flag 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Wait(request, status, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Request), INTENT(INOUT) :: request 
TYPE(MPI_Status) :: status
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Waitall(count, array_of_requests, array_of_statuses, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: count 
INTEGER, INTENT(INOUT) :: array_of_requests(*) 
TYPE(MPI_Status) :: array_of_statuses(*)
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Waitany(count, array_of_requests, index, status, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: count 
TYPE(MPI_Request), INTENT(INOUT) :: array_of_requests(*) 
INTEGER, INTENT(OUT) :: index 
TYPE(MPI_Status) :: status
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Waitsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: incount 
TYPE(MPI_Request), INTENT(INOUT) :: array_of_requests(*) 
INTEGER, INTENT(OUT) :: outcount, array_of_indices(*) 
TYPE(MPI_Status) :: array_of_statuses(*)
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Get_address(location, address, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: location 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: location
!$PRAGMA IGNORE_TKR location
REAL, DIMENSION(*) :: location
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(OUT) :: address 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 
!
!A.4.2 Datatypes Fortran Bindings
!
SUBROUTINE MPI_Get_elements(status, datatype, count, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Status), INTENT(IN) :: status
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
INTEGER, INTENT(OUT) :: count 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Pack(inbuf, incount, datatype, outbuf, outsize, position, comm, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: inbuf, outbuf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: inbuf, outbuf
!$PRAGMA IGNORE_TKR inbuf, outbuf
REAL, DIMENSION(*) :: inbuf, outbuf
INTEGER, INTENT(IN) :: incount, outsize 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
INTEGER, INTENT(INOUT) :: position 
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Pack_external(datarep, inbuf, incount, datatype, outbuf, outsize, position, ierror) 
USE mpi_f08_types
implicit none
CHARACTER(LEN=*), INTENT(IN) :: datarep 
!TYPE(*), dimension(..) :: inbuf, outbuf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: inbuf, outbuf
!$PRAGMA IGNORE_TKR inbuf, outbuf
REAL, DIMENSION(*) :: inbuf, outbuf
INTEGER, INTENT(IN) :: incount 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(IN) :: outsize 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(INOUT) :: position 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Pack_external_size(datarep, incount, datatype, size, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
INTEGER, INTENT(IN) :: incount 
CHARACTER(LEN=*), INTENT(IN) :: datarep 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(OUT) :: size 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Pack_size(incount, datatype, comm, size, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: incount 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, INTENT(OUT) :: size 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Type_commit(datatype, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Datatype), INTENT(INOUT) :: datatype 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Type_contiguous(count, oldtype, newtype, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: count 
TYPE(MPI_Datatype), INTENT(IN) :: oldtype 
TYPE(MPI_Datatype), INTENT(OUT) :: newtype 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Type_create_darray(size, rank, ndims, array_of_gsizes, &
    array_of_distribs, array_of_dargs, array_of_psizes, order, oldtype, newtype, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: size, rank, ndims, array_of_gsizes(*), array_of_distribs(*), array_of_dargs(*), array_of_psizes(*), order 
TYPE(MPI_Datatype), INTENT(IN) :: oldtype 
TYPE(MPI_Datatype), INTENT(OUT) :: newtype 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Type_create_hindexed(count, array_of_blocklengths, array_of_displacements, oldtype, newtype, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: count, array_of_blocklengths(*) 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(IN) :: array_of_displacements(*) 
TYPE(MPI_Datatype), INTENT(IN) :: oldtype 
TYPE(MPI_Datatype), INTENT(OUT) :: newtype 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Type_create_hvector(count, blocklength, stride, oldtype, newtype, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: count, blocklength 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(IN) :: stride 
TYPE(MPI_Datatype), INTENT(IN) :: oldtype 
TYPE(MPI_Datatype), INTENT(OUT) :: newtype 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Type_create_indexed_block(count, blocklength, array_of_displacements, oldtype, newtype, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: count, blocklength, array_of_displacements(*) 
TYPE(MPI_Datatype), INTENT(IN) :: oldtype 
TYPE(MPI_Datatype), INTENT(OUT) :: newtype 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Type_create_resized(oldtype, lb, extent, newtype, ierror) 
USE mpi_f08_types
implicit none
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(IN) :: lb, extent 
TYPE(MPI_Datatype), INTENT(IN) :: oldtype 
TYPE(MPI_Datatype), INTENT(OUT) :: newtype 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Type_create_struct(count, array_of_blocklengths, array_of_displacements, array_of_types, newtype, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: count, array_of_blocklengths(*) 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(IN) :: array_of_displacements(*) 
TYPE(MPI_Datatype), INTENT(IN) :: array_of_types(*) 
TYPE(MPI_Datatype), INTENT(OUT) :: newtype 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Type_create_subarray(ndims, array_of_sizes, array_of_subsizes, array_of_starts, order, oldtype, newtype, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: ndims, array_of_sizes(*), array_of_subsizes(*), array_of_starts(*), order
TYPE(MPI_Datatype), INTENT(IN) :: oldtype 
TYPE(MPI_Datatype), INTENT(OUT) :: newtype 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Type_dup(type, newtype, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Datatype), INTENT(IN) :: type 
TYPE(MPI_Datatype), INTENT(OUT) :: newtype 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 
!(This routine specification may be changed by Ticket #252-W). 

SUBROUTINE MPI_Type_free(datatype, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Datatype), INTENT(INOUT) :: datatype 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Type_get_contents(datatype, max_integers, max_addresses, &
    max_datatypes, array_of_integers, array_of_addresses, array_of_datatypes, ierror)
USE mpi_f08_types
implicit none
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
INTEGER, INTENT(IN) :: max_integers, max_addresses, max_datatypes 
INTEGER, INTENT(OUT) :: array_of_integers(*) 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(OUT) :: array_of_addresses(*) 
TYPE(MPI_Datatype), INTENT(OUT) :: array_of_datatypes(*) 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Type_get_envelope(datatype, num_integers, num_addresses, num_datatypes, combiner, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
INTEGER, INTENT(OUT) :: num_integers, num_addresses, num_datatypes, combiner 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Type_get_extent(datatype, lb, extent, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(OUT) :: lb, extent 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Type_get_true_extent(datatype, true_lb, true_extent, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(OUT) :: true_lb, true_extent 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Type_indexed(count, array_of_blocklengths, array_of_displacements, oldtype, newtype, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: count, array_of_blocklengths(*), array_of_displacements(*) 
TYPE(MPI_Datatype), INTENT(IN) :: oldtype 
TYPE(MPI_Datatype), INTENT(OUT) :: newtype 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Type_size(datatype, size, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
INTEGER, INTENT(OUT) :: size 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Type_vector(count, blocklength, stride, oldtype, newtype, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: count, blocklength, stride 
TYPE(MPI_Datatype), INTENT(IN) :: oldtype 
TYPE(MPI_Datatype), INTENT(OUT) :: newtype 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Unpack(inbuf, insize, position, outbuf, outcount, datatype, comm, ierror)
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: inbuf, outbuf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: inbuf, outbuf
!$PRAGMA IGNORE_TKR inbuf, outbuf
REAL, DIMENSION(*) :: inbuf, outbuf
INTEGER, INTENT(IN) :: insize, outcount 
INTEGER, INTENT(INOUT) :: position 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Unpack_external(datarep, inbuf, insize, position, outbuf, outcount, datatype, ierror) 
USE mpi_f08_types
implicit none
CHARACTER(LEN=*), INTENT(IN) :: datarep 
!TYPE(*), dimension(..) :: inbuf, outbuf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: inbuf, outbuf
!$PRAGMA IGNORE_TKR inbuf, outbuf
REAL, DIMENSION(*) :: inbuf, outbuf
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(IN) :: insize 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(INOUT) :: position 
INTEGER, INTENT(IN) :: outcount 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 
!
!A.4.3 Collective Communication Fortran Bindings
!
SUBROUTINE MPI_Allgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: sendbuf, recvbuf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: sendbuf, recvbuf
!$PRAGMA IGNORE_TKR sendbuf, recvbuf
REAL, DIMENSION(*) :: sendbuf, recvbuf
INTEGER, INTENT(IN) :: sendcount, recvcount 
TYPE(MPI_Datatype), INTENT(IN) :: sendtype 
TYPE(MPI_Datatype), INTENT(IN) :: recvtype 
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Allgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: sendbuf, recvbuf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: sendbuf, recvbuf
!$PRAGMA IGNORE_TKR sendbuf, recvbuf
REAL, DIMENSION(*) :: sendbuf, recvbuf
INTEGER, INTENT(IN) :: sendcount, recvcounts(*), displs(*) 
TYPE(MPI_Datatype), INTENT(IN) :: sendtype 
TYPE(MPI_Datatype), INTENT(IN) :: recvtype 
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: sendbuf, recvbuf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: sendbuf, recvbuf
!$PRAGMA IGNORE_TKR sendbuf, recvbuf
REAL, DIMENSION(*) :: sendbuf, recvbuf
INTEGER, INTENT(IN) :: count 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Op), INTENT(IN) :: op 
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Alltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: sendbuf, recvbuf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: sendbuf, recvbuf
!$PRAGMA IGNORE_TKR sendbuf, recvbuf
REAL, DIMENSION(*) :: sendbuf, recvbuf
INTEGER, INTENT(IN) :: sendcount, recvcount 
TYPE(MPI_Datatype), INTENT(IN) :: sendtype 
TYPE(MPI_Datatype), INTENT(IN) :: recvtype 
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Alltoallv(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: sendbuf, recvbuf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: sendbuf, recvbuf
!$PRAGMA IGNORE_TKR sendbuf, recvbuf
REAL, DIMENSION(*) :: sendbuf, recvbuf
INTEGER, INTENT(IN) :: sendcounts(*), sdispls(*), recvcounts(*), rdispls(*) 
TYPE(MPI_Datatype), INTENT(IN) :: sendtype 
TYPE(MPI_Datatype), INTENT(IN) :: recvtype 
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Alltoallw(sendbuf, sendcounts, sdispls, sendtypes, recvbuf, recvcounts, rdispls, recvtypes, comm, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: sendbuf, recvbuf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: sendbuf, recvbuf
!$PRAGMA IGNORE_TKR sendbuf, recvbuf
REAL, DIMENSION(*) :: sendbuf, recvbuf
INTEGER, INTENT(IN) :: sendcounts(*), sdispls(*), recvcounts(*), rdispls(*) 
TYPE(MPI_Datatype), INTENT(IN) :: sendtypes(*) 
TYPE(MPI_Datatype), INTENT(IN) :: recvtypes(*) 
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Barrier(comm, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Bcast(buffer, count, datatype, root, comm, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: buffer 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buffer
!$PRAGMA IGNORE_TKR buffer
REAL, DIMENSION(*) :: buffer
INTEGER, INTENT(IN) :: count, root 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Exscan(sendbuf, recvbuf, count, datatype, op, comm, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: sendbuf, recvbuf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: sendbuf, recvbuf
!$PRAGMA IGNORE_TKR sendbuf, recvbuf
REAL, DIMENSION(*) :: sendbuf, recvbuf
INTEGER, INTENT(IN) :: count 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Op), INTENT(IN) :: op 
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Gather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: sendbuf, recvbuf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: sendbuf, recvbuf
!$PRAGMA IGNORE_TKR sendbuf, recvbuf
REAL, DIMENSION(*) :: sendbuf, recvbuf
INTEGER, INTENT(IN) :: sendcount, recvcount, root 
TYPE(MPI_Datatype), INTENT(IN) :: sendtype 
TYPE(MPI_Datatype), INTENT(IN) :: recvtype 
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Gatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: sendbuf, recvbuf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: sendbuf, recvbuf
!$PRAGMA IGNORE_TKR sendbuf, recvbuf
REAL, DIMENSION(*) :: sendbuf, recvbuf
INTEGER, INTENT(IN) :: sendcount, recvcounts(*), displs(*), root 
TYPE(MPI_Datatype), INTENT(IN) :: sendtype 
TYPE(MPI_Datatype), INTENT(IN) :: recvtype 
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Op_commutative(op, commute, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Op), INTENT(IN) :: op 
LOGICAL, INTENT(OUT) :: commute 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Op_create(func, commute, op, ierror) 
USE mpi_f08_types
implicit none
PROCEDURE() :: func
LOGICAL, INTENT(IN) :: commute 
TYPE(MPI_Op), INTENT(OUT) :: op 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Op_free(op, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Op), INTENT(INOUT) :: op 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Reduce(sendbuf, recvbuf, count, datatype, op, root, comm, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: sendbuf, recvbuf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: sendbuf, recvbuf
!$PRAGMA IGNORE_TKR sendbuf, recvbuf
REAL, DIMENSION(*) :: sendbuf, recvbuf
INTEGER, INTENT(IN) :: count, root 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Op), INTENT(IN) :: op 
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Reduce_local(inbuf, inoutbuf, count, datatype, op, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: inbuf, inoutbuf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: inbuf, inoutbuf
!$PRAGMA IGNORE_TKR inbuf, inoutbuf
REAL, DIMENSION(*) :: inbuf, inoutbuf
INTEGER, INTENT(IN) :: count 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Op), INTENT(IN) :: op 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Reduce_scatter(sendbuf, recvbuf, recvcounts, datatype, op, comm, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: sendbuf, recvbuf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: sendbuf, recvbuf
!$PRAGMA IGNORE_TKR sendbuf, recvbuf
REAL, DIMENSION(*) :: sendbuf, recvbuf
INTEGER, INTENT(IN) :: recvcounts(*) 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Op), INTENT(IN) :: op 
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Reduce_scatter_block(sendbuf, recvbuf, recvcount, datatype, op, comm, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: sendbuf, recvbuf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: sendbuf, recvbuf
!$PRAGMA IGNORE_TKR sendbuf, recvbuf
REAL, DIMENSION(*) :: sendbuf, recvbuf
INTEGER, INTENT(IN) :: recvcount 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Op), INTENT(IN) :: op 
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Scan(sendbuf, recvbuf, count, datatype, op, comm, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: sendbuf, recvbuf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: sendbuf, recvbuf
!$PRAGMA IGNORE_TKR sendbuf, recvbuf
REAL, DIMENSION(*) :: sendbuf, recvbuf
INTEGER, INTENT(IN) :: count 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Op), INTENT(IN) :: op 
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Scatter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: sendbuf, recvbuf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: sendbuf, recvbuf
!$PRAGMA IGNORE_TKR sendbuf, recvbuf
REAL, DIMENSION(*) :: sendbuf, recvbuf
INTEGER, INTENT(IN) :: sendcount, recvcount, root 
TYPE(MPI_Datatype), INTENT(IN) :: sendtype 
TYPE(MPI_Datatype), INTENT(IN) :: recvtype 
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Scatterv(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: sendbuf, recvbuf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: sendbuf, recvbuf
!$PRAGMA IGNORE_TKR sendbuf, recvbuf
REAL, DIMENSION(*) :: sendbuf, recvbuf
INTEGER, INTENT(IN) :: sendcounts(*), displs(*), recvcount, root 
TYPE(MPI_Datatype), INTENT(IN) :: sendtype 
TYPE(MPI_Datatype), INTENT(IN) :: recvtype 
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 
!
!A.4.4 Groups, Contexts, Communicators, and Caching Fortran Bindings
!
SUBROUTINE MPI_Comm_compare(comm1, comm2, result, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm1 
TYPE(MPI_Comm), INTENT(IN) :: comm2 
INTEGER, INTENT(OUT) :: result 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Comm_create(comm, group, newcomm, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
TYPE(MPI_Group), INTENT(IN) :: group 
TYPE(MPI_Comm), INTENT(OUT) :: newcomm 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Comm_create_keyval(comm_copy_attr_fn, comm_delete_attr_fn, comm_keyval, extra_state, ierror) 
USE mpi_f08_types
implicit none
INTERFACE
  SUBROUTINE comm_copy_attr_fn(oldcomm, comm_keyval, extra_state, attribute_val_in, attribute_val_out, flag, ierror)
   USE mpi_f08_types
   TYPE(MPI_Comm), INTENT(IN) :: oldcomm
   INTEGER, INTENT(IN) :: comm_keyval
   !TYPE(*), INTENT(IN) :: extra_state, attribute_val_in
   !TYPE(*), INTENT(OUT) :: attribute_val_out
   !DEC$ ATTRIBUTES NO_ARG_CHECK :: extra_state, attribute_val_in attribute_val_in
   !$PRAGMA IGNORE_TKR extra_state, attribute_val_in attribute_val_in
   REAL, INTENT(IN) :: extra_state, attribute_val_in
   REAL, INTENT(OUT) :: attribute_val_out
   LOGICAL, INTENT(OUT) :: flag
   INTEGER, INTENT(OUT) :: ierror
  END SUBROUTINE
  SUBROUTINE comm_delete_attr_fn(comm, comm_keyval, attribute_val, extra_state, ierror)
   USE mpi_f08_types
   TYPE(MPI_Comm), INTENT(IN) :: comm
   INTEGER, INTENT(IN) :: comm_keyval
   !TYPE(*), INTENT(OUT) :: attribute_val, extra_state
   !DEC$ ATTRIBUTES NO_ARG_CHECK :: attribute_val, extra_state
   !$PRAGMA IGNORE_TKR attribute_val, extra_state
   REAL, INTENT(OUT) :: attribute_val, extra_state
   INTEGER, INTENT(OUT) :: ierror
  END SUBROUTINE
END INTERFACE
INTEGER, INTENT(OUT) :: comm_keyval 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(IN) :: extra_state 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Comm_delete_attr(comm, comm_keyval, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, INTENT(IN) :: comm_keyval 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Comm_dup(comm, newcomm, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
TYPE(MPI_Comm), INTENT(OUT) :: newcomm 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_COMM_DUP_FN(oldcomm, comm_keyval, extra_state, attribute_val_in, attribute_val_out, flag, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: oldcomm 
INTEGER, INTENT(IN) :: comm_keyval 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(IN) :: extra_state, attribute_val_in 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(OUT) :: attribute_val_out 
LOGICAL, INTENT(OUT) :: flag 
INTEGER, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Comm_free(comm, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(INOUT) :: comm 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Comm_free_keyval(comm_keyval, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(INOUT) :: comm_keyval 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Comm_get_attr(comm, comm_keyval, attribute_val, flag, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, INTENT(IN) :: comm_keyval 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(OUT) :: attribute_val 
LOGICAL, INTENT(OUT) :: flag 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Comm_get_name(comm, comm_name, resultlen, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
CHARACTER(LEN=*), INTENT(OUT) :: comm_name 
INTEGER, INTENT(OUT) :: resultlen 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Comm_group(comm, group, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
TYPE(MPI_Group), INTENT(OUT) :: group 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_COMM_NULL_COPY_FN(oldcomm, comm_keyval, extra_state, attribute_val_in, attribute_val_out, flag, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: oldcomm 
INTEGER, INTENT(IN) :: comm_keyval 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(IN) :: extra_state, attribute_val_in 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(OUT) :: attribute_val_out 
LOGICAL, INTENT(OUT) :: flag 
INTEGER, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_COMM_NULL_DELETE_FN(comm, comm_keyval, attribute_val, extra_state, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, INTENT(IN) :: comm_keyval 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(IN) :: attribute_val, extra_state 
INTEGER, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Comm_rank(comm, rank, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, INTENT(OUT) :: rank 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Comm_remote_group(comm, group, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
TYPE(MPI_Group), INTENT(OUT) :: group 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Comm_remote_size(comm, size, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, INTENT(OUT) :: size 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Comm_set_attr(comm, comm_keyval, attribute_val, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, INTENT(IN) :: comm_keyval 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(IN) :: attribute_val 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Comm_set_name(comm, comm_name, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
CHARACTER(LEN=*), INTENT(IN) :: comm_name 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Comm_size(comm, size, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, INTENT(OUT) :: size 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Comm_split(comm, color, key, newcomm, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, INTENT(IN) :: color, key 
TYPE(MPI_Comm), INTENT(OUT) :: newcomm 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Comm_test_inter(comm, flag, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
LOGICAL, INTENT(OUT) :: flag 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Group_compare(group1, group2, result, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Group), INTENT(IN) :: group1 
TYPE(MPI_Group), INTENT(IN) :: group2 
INTEGER, INTENT(OUT) :: result 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Group_difference(group1, group2, newgroup, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Group), INTENT(IN) :: group1 
TYPE(MPI_Group), INTENT(IN) :: group2 
TYPE(MPI_Group), INTENT(OUT) :: newgroup 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Group_excl(group, n, ranks, newgroup, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Group), INTENT(IN) :: group 
INTEGER, INTENT(IN) :: n, ranks(*) 
TYPE(MPI_Group), INTENT(OUT) :: newgroup 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Group_free(group, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Group), INTENT(INOUT) :: group 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Group_incl(group, n, ranks, newgroup, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: n, ranks(*) 
TYPE(MPI_Group), INTENT(IN) :: group 
TYPE(MPI_Group), INTENT(OUT) :: newgroup 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Group_intersection(group1, group2, newgroup, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Group), INTENT(IN) :: group1 
TYPE(MPI_Group), INTENT(IN) :: group2 
TYPE(MPI_Group), INTENT(OUT) :: newgroup 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Group_range_excl(group, n, ranges, newgroup, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Group), INTENT(IN) :: group 
INTEGER, INTENT(IN) :: n, ranges(3,*) 
TYPE(MPI_Group), INTENT(OUT) :: newgroup 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Group_range_incl(group, n, ranges, newgroup, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Group), INTENT(IN) :: group 
INTEGER, INTENT(IN) :: n, ranges(3,*) 
TYPE(MPI_Group), INTENT(OUT) :: newgroup 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Group_rank(group, rank, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Group), INTENT(IN) :: group 
INTEGER, INTENT(OUT) :: rank 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Group_size(group, size, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Group), INTENT(IN) :: group 
INTEGER, INTENT(OUT) :: size 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Group_translate_ranks(group1, n, ranks1, group2, ranks2, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Group), INTENT(IN) :: group1, group2 
INTEGER, INTENT(IN) :: n, ranks1(*) 
INTEGER, INTENT(OUT) :: ranks2(*) 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Group_union(group1, group2, newgroup, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Group), INTENT(IN) :: group1, group2 
TYPE(MPI_Group), INTENT(OUT) :: newgroup 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Intercomm_create(local_comm, local_leader, peer_comm, remote_leader, tag, newintercomm, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: local_comm, peer_comm 
INTEGER, INTENT(IN) :: local_leader, remote_leader, tag 
TYPE(MPI_Comm), INTENT(OUT) :: newintercomm 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Intercomm_merge(intercomm, high, newintracomm, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: intercomm 
LOGICAL, INTENT(IN) :: high 
TYPE(MPI_Comm), INTENT(OUT) :: newintracomm 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Type_create_keyval(type_copy_attr_fn, type_delete_attr_fn, type_keyval, extra_state, ierror) 
USE mpi_f08_types
implicit none
PROCEDURE() :: type_copy_attr_fn, type_delete_attr_fn 
INTEGER, INTENT(OUT) :: type_keyval 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(IN) :: extra_state 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Type_delete_attr(type, type_keyval, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Datatype), INTENT(IN) :: type 
INTEGER, INTENT(IN) :: type_keyval 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 
!(This routine specification may be changed by Ticket #252-W). 

SUBROUTINE MPI_TYPE_DUP_FN(oldtype, type_keyval, extra_state, attribute_val_in, attribute_val_out, flag, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Datatype), INTENT(IN) :: oldtype 
INTEGER, INTENT(IN) :: type_keyval 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(IN) :: extra_state, attribute_val_in 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(OUT) :: attribute_val_out 
LOGICAL, INTENT(OUT) :: flag 
INTEGER, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Type_free_keyval(type_keyval, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(INOUT) :: type_keyval 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Type_get_attr(type, type_keyval, attribute_val, flag, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Datatype), INTENT(IN) :: type 
INTEGER, INTENT(IN) :: type_keyval 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(OUT) :: attribute_val 
LOGICAL, INTENT(OUT) :: flag 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 
!(This routine specification may be changed by Ticket #252-W). 

SUBROUTINE MPI_Type_get_name(type, type_name, resultlen, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Datatype), INTENT(IN) :: type 
CHARACTER(LEN=*), INTENT(OUT) :: type_name 
INTEGER, INTENT(OUT) :: resultlen 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 
!(This routine specification may be changed by Ticket #252-W). 

SUBROUTINE MPI_TYPE_NULL_COPY_FN(oldtype, type_keyval, extra_state, attribute_val_in, attribute_val_out, flag, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Datatype), INTENT(IN) :: oldtype 
INTEGER, INTENT(IN) :: type_keyval 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(IN) :: extra_state, attribute_val_in 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(OUT) :: attribute_val_out 
LOGICAL, INTENT(OUT) :: flag 
INTEGER, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_TYPE_NULL_DELETE_FN(type, type_keyval, attribute_val, extra_state, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Datatype), INTENT(IN) :: type 
INTEGER, INTENT(IN) :: type_keyval 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(IN) :: attribute_val, extra_state 
INTEGER, INTENT(OUT) :: ierror 
END SUBROUTINE 
!(This routine specification may be changed by Ticket #252-W). 

SUBROUTINE MPI_Type_set_attr(type, type_keyval, attribute_val, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Datatype), INTENT(IN) :: type 
INTEGER, INTENT(IN) :: type_keyval 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(IN) :: attribute_val 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 
!(This routine specification may be changed by Ticket #252-W). 

SUBROUTINE MPI_Type_set_name(type, type_name, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Datatype), INTENT(IN) :: type 
CHARACTER(LEN=*), INTENT(IN) :: type_name 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 
!(This routine specification may be changed by Ticket #252-W). 

SUBROUTINE MPI_Win_create_keyval(win_copy_attr_fn, win_delete_attr_fn, win_keyval, extra_state, ierror) 
USE mpi_f08_types
implicit none
PROCEDURE() :: win_copy_attr_fn, win_delete_attr_fn 
INTEGER, INTENT(OUT) :: win_keyval 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(IN) :: extra_state 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Win_delete_attr(win, win_keyval, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Win), INTENT(IN) :: win 
INTEGER, INTENT(IN) :: win_keyval 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_WIN_DUP_FN(oldwin, win_keyval, extra_state, attribute_val_in, attribute_val_out, flag, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: oldwin, win_keyval 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(IN) :: extra_state, attribute_val_in 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(OUT) :: attribute_val_out 
LOGICAL, INTENT(OUT) :: flag 
INTEGER, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Win_free_keyval(win_keyval, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(INOUT) :: win_keyval 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Win_get_attr(win, win_keyval, attribute_val, flag, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Win), INTENT(IN) :: win 
INTEGER, INTENT(IN) :: win_keyval 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(OUT) :: attribute_val 
LOGICAL, INTENT(OUT) :: flag 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Win_get_name(win, win_name, resultlen, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Win), INTENT(IN) :: win 
CHARACTER(LEN=*), INTENT(OUT) :: win_name 
INTEGER, INTENT(OUT) :: resultlen 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_WIN_NULL_COPY_FN(oldwin, win_keyval, extra_state, attribute_val_in, attribute_val_out, flag, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: oldwin, win_keyval 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(IN) :: extra_state, attribute_val_in 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(OUT) :: attribute_val_out 
LOGICAL, INTENT(OUT) :: flag 
INTEGER, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_WIN_NULL_DELETE_FN(win, win_keyval, attribute_val, extra_state, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Win), INTENT(IN) :: win 
INTEGER, INTENT(IN) :: win_keyval 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(IN) :: attribute_val, extra_state 
INTEGER, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Win_set_attr(win, win_keyval, attribute_val, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Win), INTENT(IN) :: win 
INTEGER, INTENT(IN) :: win_keyval 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(IN) :: attribute_val 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Win_set_name(win, win_name, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Win), INTENT(IN) :: win 
CHARACTER(LEN=*), INTENT(IN) :: win_name 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 
!
!A.4.5 Process Topologies Fortran Bindings
!
SUBROUTINE MPI_Cartdim_get(comm, ndims, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, INTENT(OUT) :: ndims 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Cart_coords(comm, rank, maxdims, coords, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, INTENT(IN) :: rank, maxdims 
INTEGER, INTENT(OUT) :: coords(*) 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Cart_create(comm_old, ndims, dims, periods, reorder, comm_cart, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm_old 
INTEGER, INTENT(IN) :: ndims, dims(*) 
LOGICAL, INTENT(IN) :: periods(*), reorder 
TYPE(MPI_Comm), INTENT(OUT) :: comm_cart 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Cart_get(comm, maxdims, dims, periods, coords, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, INTENT(IN) :: maxdims 
INTEGER, INTENT(OUT) :: dims(*), coords(*) 
LOGICAL, INTENT(OUT) :: periods(*) 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Cart_map(comm, ndims, dims, periods, newrank, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, INTENT(IN) :: ndims, dims(*) 
LOGICAL, INTENT(IN) :: periods(*) 
INTEGER, INTENT(OUT) :: newrank 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Cart_rank(comm, coords, rank, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, INTENT(IN) :: coords(*) 
INTEGER, INTENT(OUT) :: rank 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Cart_shift(comm, direction, disp, rank_source, rank_dest, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, INTENT(IN) :: direction, disp 
INTEGER, INTENT(OUT) :: rank_source, rank_dest 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Cart_sub(comm, remain_dims, newcomm, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
LOGICAL, INTENT(IN) :: remain_dims(*) 
TYPE(MPI_Comm), INTENT(OUT) :: newcomm 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Dims_create(nnodes, ndims, dims, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: nnodes, ndims 
INTEGER, INTENT(INOUT) :: dims(*) 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Dist_graph_create(comm_old, n, sources, degrees, destinations, weights, info, reorder, comm_dist_graph, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm_old 
INTEGER, INTENT(IN) :: n, sources(*), degrees(*), destinations(*), weights(*) 
TYPE(MPI_Info), INTENT(IN) :: info 
LOGICAL, INTENT(IN) :: reorder 
TYPE(MPI_Comm), INTENT(OUT) :: comm_dist_graph 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Dist_graph_create_adjacent(comm_old, indegree, sources, sourceweights, &
    outdegree, destinations, destweights, info, reorder, comm_dist_graph, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm_old 
INTEGER, INTENT(IN) :: indegree, sources(*), sourceweights(*), outdegree, destinations(*), destweights(*) 
TYPE(MPI_Info), INTENT(IN) :: info 
LOGICAL, INTENT(IN) :: reorder 
TYPE(MPI_Comm), INTENT(OUT) :: comm_dist_graph 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Dist_graph_neighbors(comm, maxindegree, sources, sourceweights, maxoutdegree, destinations, destweights, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, INTENT(IN) :: maxindegree, maxoutdegree 
INTEGER, INTENT(OUT) :: sources(*), destinations(*) 
INTEGER :: sourceweights(*), destweights(*) 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Dist_graph_neighbors_count(comm, indegree, outdegree, weighted, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, INTENT(OUT) :: indegree, outdegree 
LOGICAL, INTENT(OUT) :: weighted 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Graphdims_get(comm, nnodes, nedges, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, INTENT(OUT) :: nnodes, nedges 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Graph_create(comm_old, nnodes, index, edges, reorder, comm_graph, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm_old 
INTEGER, INTENT(IN) :: nnodes, index(*), edges(*) 
LOGICAL, INTENT(IN) :: reorder 
TYPE(MPI_Comm), INTENT(OUT) :: comm_graph 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Graph_get(comm, maxindex, maxedges, index, edges, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, INTENT(IN) :: maxindex, maxedges 
INTEGER, INTENT(OUT) :: index(*), edges(*) 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Graph_map(comm, nnodes, index, edges, newrank, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, INTENT(IN) :: nnodes, index(*), edges(*) 
INTEGER, INTENT(OUT) :: newrank 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Graph_neighbors(comm, rank, maxneighbors, neighbors, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, INTENT(IN) :: rank, maxneighbors 
INTEGER, INTENT(OUT) :: neighbors(*) 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Graph_neighbors_count(comm, rank, nneighbors, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, INTENT(IN) :: rank 
INTEGER, INTENT(OUT) :: nneighbors 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Topo_test(comm, status, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
TYPE(MPI_Status), INTENT(OUT) :: status
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 
!
!A.4.6 MPI Environmenta Management Fortran Bindings
!
DOUBLE PRECISION FUNCTION MPI_Wtick() 
END FUNCTION

DOUBLE PRECISION FUNCTION MPI_Wtime() 
END FUNCTION

SUBROUTINE MPI_Abort(comm, errorcode, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, INTENT(IN) :: errorcode 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Add_error_class(errorclass, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(OUT) :: errorclass 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Add_error_code(errorclass, errorcode, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: errorclass 
INTEGER, INTENT(OUT) :: errorcode 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Add_error_string(errorcode, string, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: errorcode 
CHARACTER(LEN=*), INTENT(IN) :: string 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Alloc_mem(size, info, baseptr, ierror) 
USE mpi_f08_types
implicit none
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(IN) :: size 
TYPE(MPI_Info), INTENT(IN) :: info 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(OUT) :: baseptr 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Comm_call_errhandler(comm, errorcode, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
INTEGER, INTENT(IN) :: errorcode 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Comm_create_errhandler(func, errhandler, ierror) 
USE mpi_f08_types
implicit none
PROCEDURE() :: func
TYPE(MPI_Errhandler), INTENT(OUT) :: errhandler 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Comm_get_errhandler(comm, errhandler, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
TYPE(MPI_Errhandler), INTENT(OUT) :: errhandler 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Comm_set_errhandler(comm, errhandler, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
TYPE(MPI_Errhandler), INTENT(IN) :: errhandler 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Errhandler_free(errhandler, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Errhandler), INTENT(INOUT) :: errhandler 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Error_class(errorcode, errorclass, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: errorcode 
INTEGER, INTENT(OUT) :: errorclass 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Error_string(errorcode, string, resultlen, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: errorcode 
CHARACTER(LEN=*), INTENT(OUT) :: string 
INTEGER, INTENT(OUT) :: resultlen 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_call_errhandler(fh, errorcode, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
INTEGER, INTENT(IN) :: errorcode 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_create_errhandler(func, errhandler, ierror) 
USE mpi_f08_types
implicit none
PROCEDURE() :: func
TYPE(MPI_Errhandler), INTENT(OUT) :: errhandler 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_get_errhandler(file, errhandler, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: file 
TYPE(MPI_Errhandler), INTENT(OUT) :: errhandler 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_set_errhandler(file, errhandler, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: file 
TYPE(MPI_Errhandler), INTENT(IN) :: errhandler 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Finalize(ierror) 
USE mpi_f08_types
implicit none
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Finalized(flag, ierror) 
USE mpi_f08_types
implicit none
LOGICAL, INTENT(OUT) :: flag 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Free_mem(base, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: base 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: base
!$PRAGMA IGNORE_TKR base
REAL, DIMENSION(*) :: base
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Get_processor_name( name, resultlen, ierror) 
USE mpi_f08_types
implicit none
CHARACTER(LEN=*), INTENT(OUT) :: name 
INTEGER, INTENT(OUT) :: resultlen 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Get_version(version, subversion, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(OUT) :: version, subversion 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Init(ierror) 
USE mpi_f08_types
implicit none
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Initialized(flag, ierror) 
USE mpi_f08_types
implicit none
LOGICAL, INTENT(OUT) :: flag 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Win_call_errhandler(win, errorcode, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Win), INTENT(IN) :: win 
INTEGER, INTENT(IN) :: errorcode 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Win_create_errhandler(func, errhandler, ierror) 
USE mpi_f08_types
implicit none
PROCEDURE() :: func
TYPE(MPI_Errhandler), INTENT(OUT) :: errhandler 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Win_get_errhandler(win, errhandler, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Win), INTENT(IN) :: win 
TYPE(MPI_Errhandler), INTENT(OUT) :: errhandler 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Win_set_errhandler(win, errhandler, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Win), INTENT(IN) :: win 
TYPE(MPI_Errhandler), INTENT(IN) :: errhandler 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 
!
!A.3.7 The Info Object Fortran Bindings
!
SUBROUTINE MPI_Info_create(info, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Info), INTENT(OUT) :: info 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Info_delete(info, key, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Info), INTENT(IN) :: info 
CHARACTER(LEN=*), INTENT(IN) :: key 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Info_dup(info, newinfo, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Info), INTENT(IN) :: info 
TYPE(MPI_Info), INTENT(OUT) :: newinfo 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Info_free(info, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Info), INTENT(INOUT) :: info 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Info_get(info, key, valuelen, value, flag, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Info), INTENT(IN) :: info 
CHARACTER(LEN=*), INTENT(IN) :: key 
INTEGER, INTENT(IN) :: valuelen 
CHARACTER(LEN=*), INTENT(OUT) :: value 
LOGICAL, INTENT(OUT) :: flag 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Info_get_nkeys(info, nkeys, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Info), INTENT(IN) :: info 
INTEGER, INTENT(OUT) :: nkeys 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Info_get_nthkey(info, n, key, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Info), INTENT(IN) :: info 
INTEGER, INTENT(IN) :: n 
CHARACTER(LEN=*), INTENT(OUT) :: key 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Info_get_valuelen(info, key, valuelen, flag, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Info), INTENT(IN) :: info 
CHARACTER(LEN=*), INTENT(IN) :: key 
INTEGER, INTENT(OUT) :: valuelen 
LOGICAL, INTENT(OUT) :: flag 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Info_set(info, key, value, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Info), INTENT(IN) :: info 
CHARACTER(LEN=*), INTENT(IN) :: key, value 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 
!
!A.3.8 Process Creation and Management Fortran Bindings
!
SUBROUTINE MPI_Close_port(port_name, ierror) 
USE mpi_f08_types
implicit none
CHARACTER(LEN=*), INTENT(IN) :: port_name 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Comm_accept(port_name, info, root, comm, newcomm, ierror) 
USE mpi_f08_types
implicit none
CHARACTER(LEN=*), INTENT(IN) :: port_name 
TYPE(MPI_Info), INTENT(IN) :: info 
INTEGER, INTENT(IN) :: root 
TYPE(MPI_Comm), INTENT(IN) :: comm 
TYPE(MPI_Comm), INTENT(OUT) :: newcomm 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Comm_connect(port_name, info, root, comm, newcomm, ierror) 
USE mpi_f08_types
implicit none
CHARACTER(LEN=*), INTENT(IN) :: port_name 
TYPE(MPI_Info), INTENT(IN) :: info 
INTEGER, INTENT(IN) :: root 
TYPE(MPI_Comm), INTENT(IN) :: comm 
TYPE(MPI_Comm), INTENT(OUT) :: newcomm 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Comm_disconnect(comm, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(INOUT) :: comm 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Comm_get_parent(parent, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(OUT) :: parent 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Comm_join(fd, intercomm, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: fd 
TYPE(MPI_Comm), INTENT(OUT) :: intercomm 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Comm_spawn(command, argv, maxprocs, info, root, comm, intercomm, array_of_errcodes, ierror) 
USE mpi_f08_types
implicit none
CHARACTER(LEN=*), INTENT(IN) :: command, argv(*) 
INTEGER, INTENT(IN) :: maxprocs, root 
TYPE(MPI_Info), INTENT(IN) :: info 
TYPE(MPI_Comm), INTENT(IN) :: comm 
TYPE(MPI_Comm), INTENT(OUT) :: intercomm 
INTEGER :: array_of_errcodes(*) 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Comm_spawn_multiple(count, array_of_commands, array_of_argv, &
    array_of_maxprocs, array_of_info, root, comm, intercomm, array_of_errcodes, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: count, array_of_maxprocs(*), root 
CHARACTER(LEN=*), INTENT(IN) :: array_of_commands(*), array_of_argv(count, *) 
TYPE(MPI_Info), INTENT(IN) :: array_of_info(*) 
TYPE(MPI_Comm), INTENT(IN) :: comm 
TYPE(MPI_Comm), INTENT(OUT) :: intercomm 
INTEGER :: array_of_errcodes(*) 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Lookup_name(service_name, info, port_name, ierror) 
USE mpi_f08_types
implicit none
CHARACTER(LEN=*), INTENT(IN) :: service_name
TYPE(MPI_Info), INTENT(IN) :: info 
CHARACTER(LEN=*), INTENT(OUT) :: port_name 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Open_port(info, port_name, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Info), INTENT(IN) :: info 
CHARACTER(LEN=*), INTENT(OUT) :: port_name 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Publish_name(service_name, info, port_name, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Info), INTENT(IN) :: info 
CHARACTER(LEN=*), INTENT(IN) :: service_name, port_name 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Unpublish_name(service_name, info, port_name, ierror) 
USE mpi_f08_types
implicit none
CHARACTER(LEN=*), INTENT(IN) :: service_name, port_name 
TYPE(MPI_Info), INTENT(IN) :: info 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 
!
!A.4.9 One-Sided Communications Fortran Bindings
!
SUBROUTINE MPI_Accumulate(origin_addr, origin_count, origin_datatype, target_rank, &
    target_disp, target_count, target_datatype, op, win, ierror)
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: origin_addr 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: origin_addr
!$PRAGMA IGNORE_TKR origin_addr
REAL, DIMENSION(*) :: origin_addr
INTEGER, INTENT(IN) :: origin_count, target_rank, target_count 
TYPE(MPI_Datatype), INTENT(IN) :: origin_datatype 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(IN) :: target_disp 
TYPE(MPI_Datatype), INTENT(IN) :: target_datatype 
TYPE(MPI_Op), INTENT(IN) :: op 
TYPE(MPI_Win), INTENT(IN) :: win 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Get(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: origin_addr 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: origin_addr
!$PRAGMA IGNORE_TKR origin_addr
REAL, DIMENSION(*) :: origin_addr
INTEGER, INTENT(IN) :: origin_count, target_rank, target_count 
TYPE(MPI_Datatype), INTENT(IN) :: origin_datatype 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(IN) :: target_disp 
TYPE(MPI_Datatype), INTENT(IN) :: target_datatype 
TYPE(MPI_Win), INTENT(IN) :: win 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Put(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: origin_addr 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: origin_addr
!$PRAGMA IGNORE_TKR origin_addr
REAL, DIMENSION(*) :: origin_addr
INTEGER, INTENT(IN) :: origin_count, target_rank, target_count 
TYPE(MPI_Datatype), INTENT(IN) :: origin_datatype 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(IN) :: target_disp 
TYPE(MPI_Datatype), INTENT(IN) :: target_datatype 
TYPE(MPI_Win), INTENT(IN) :: win 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Win_complete(win, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Win), INTENT(IN) :: win 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Win_create(base, size, disp_unit, info, comm, win, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: base 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: base
!$PRAGMA IGNORE_TKR base
REAL, DIMENSION(*) :: base
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(IN) :: size 
INTEGER, INTENT(IN) :: disp_unit 
TYPE(MPI_Info), INTENT(IN) :: info 
TYPE(MPI_Comm), INTENT(IN) :: comm 
TYPE(MPI_Win), INTENT(OUT) :: win 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Win_fence(assert, win, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: assert 
TYPE(MPI_Win), INTENT(IN) :: win 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Win_free(win, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Win), INTENT(INOUT) :: win 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Win_get_group(win, group, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Win), INTENT(IN) :: win 
TYPE(MPI_Group), INTENT(OUT) :: group 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Win_lock(lock_type, rank, assert, win, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: lock_type, rank, assert 
TYPE(MPI_Win), INTENT(IN) :: win 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Win_post(group, assert, win, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Group), INTENT(IN) :: group 
INTEGER, INTENT(IN) :: assert 
TYPE(MPI_Win), INTENT(IN) :: win 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Win_start(group, assert, win, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Group), INTENT(IN) :: group 
INTEGER, INTENT(IN) :: assert 
TYPE(MPI_Win), INTENT(IN) :: win 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Win_test(win, flag, ierror) 
USE mpi_f08_types
implicit none
LOGICAL, INTENT(OUT) :: flag 
TYPE(MPI_Win), INTENT(IN) :: win 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Win_unlock(rank, win, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: rank 
TYPE(MPI_Win), INTENT(IN) :: win 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Win_wait(win, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Win), INTENT(IN) :: win 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 
!
!A.4.10 External Interfaces Fortran Bindings
!
SUBROUTINE MPI_Grequest_complete(request, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Request), INTENT(IN) :: request 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Grequest_start(query_fn, free_fn, cancel_fn, extra_state, request, ierror) 
USE mpi_f08_types
implicit none
PROCEDURE() :: query_fn, free_fn, cancel_fn 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(IN) :: extra_state 
TYPE(MPI_Request), INTENT(OUT) :: request 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Init_thread(required, provided, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: required 
INTEGER, INTENT(OUT) :: provided 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Is_thread_main(flag, ierror) 
USE mpi_f08_types
implicit none
LOGICAL, INTENT(OUT) :: flag 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Query_thread(provided, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(OUT) :: provided 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Status_set_cancelled(status, flag, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Status), INTENT(INOUT) :: status
LOGICAL, INTENT(OUT) :: flag 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Status_set_elements(status, datatype, count, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Status), INTENT(INOUT) :: status
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
INTEGER, INTENT(IN) :: count 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 
!
!A.4.11 I/O Fortran Bindings
!
SUBROUTINE MPI_File_close(fh, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(INOUT) :: fh 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_delete(filename, info, ierror) 
USE mpi_f08_types
implicit none
CHARACTER(LEN=*), INTENT(IN) :: filename 
TYPE(MPI_Info), INTENT(IN) :: info 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_get_amode(fh, amode, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
INTEGER, INTENT(OUT) :: amode 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_get_atomicity(fh, flag, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
LOGICAL, INTENT(OUT) :: flag 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_get_byte_offset(fh, offset, disp, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
INTEGER(KIND=MPI_OFFSET_KIND), INTENT(IN) :: offset 
INTEGER(KIND=MPI_OFFSET_KIND), INTENT(OUT) :: disp 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_get_group(fh, group, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
TYPE(MPI_Group), INTENT(OUT) :: group 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_get_info(fh, info_used, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
TYPE(MPI_Info), INTENT(OUT) :: info_used 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_get_position(fh, offset, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
INTEGER(KIND=MPI_OFFSET_KIND), INTENT(OUT) :: offset 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_get_position_shared(fh, offset, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
INTEGER(KIND=MPI_OFFSET_KIND), INTENT(OUT) :: offset 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_get_size(fh, size, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
INTEGER(KIND=MPI_OFFSET_KIND), INTENT(OUT) :: size 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_get_type_extent(fh, datatype, extent, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(OUT) :: extent 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_get_view(fh, disp, etype, filetype, datarep, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
INTEGER(KIND=MPI_OFFSET_KIND), INTENT(OUT) :: disp 
TYPE(MPI_Datatype), INTENT(OUT) :: etype 
TYPE(MPI_Datatype), INTENT(OUT) :: filetype 
CHARACTER(LEN=*), INTENT(OUT) :: datarep 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_iread(fh, buf, count, datatype, request, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Request), INTENT(OUT) :: request 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_iread_at(fh, offset, buf, count, datatype, request, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
INTEGER(KIND=MPI_OFFSET_KIND), INTENT(IN) :: offset 
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Request), INTENT(OUT) :: request 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_iread_shared(fh, buf, count, datatype, request, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Request), INTENT(OUT) :: request 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_iwrite(fh, buf, count, datatype, request, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Request), INTENT(OUT) :: request 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_iwrite_at(fh, offset, buf, count, datatype, request, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
INTEGER(KIND=MPI_OFFSET_KIND), INTENT(IN) :: offset 
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Request), INTENT(OUT) :: request 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_iwrite_shared(fh, buf, count, datatype, request, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
TYPE(MPI_File), INTENT(IN) :: fh 
INTEGER, INTENT(IN) :: count 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Request), INTENT(OUT) :: request 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_open(comm, filename, amode, info, fh, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_Comm), INTENT(IN) :: comm 
CHARACTER(LEN=*), INTENT(IN) :: filename 
INTEGER, INTENT(IN) :: amode 
TYPE(MPI_Info), INTENT(IN) :: info 
TYPE(MPI_File), INTENT(OUT) :: fh 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_preallocate(fh, size, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
INTEGER(KIND=MPI_OFFSET_KIND), INTENT(IN) :: size 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_read(fh, buf, count, datatype, status, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Status) :: status
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_read_all(fh, buf, count, datatype, status, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Status) :: status
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_read_all_begin(fh, buf, count, datatype, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_read_all_end(fh, buf, status, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
TYPE(MPI_Status) :: status
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_read_at(fh, offset, buf, count, datatype, status, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
INTEGER(KIND=MPI_OFFSET_KIND), INTENT(IN) :: offset 
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Status) :: status
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_read_at_all(fh, offset, buf, count, datatype, status, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
INTEGER(KIND=MPI_OFFSET_KIND), INTENT(IN) :: offset 
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Status) :: status
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_read_at_all_begin(fh, offset, buf, count, datatype, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
INTEGER(KIND=MPI_OFFSET_KIND), INTENT(IN) :: offset 
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_read_at_all_end(fh, buf, status, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
TYPE(MPI_Status) :: status
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_read_ordered(fh, buf, count, datatype, status, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Status) :: status
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_read_ordered_begin(fh, buf, count, datatype, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_read_ordered_end(fh, buf, status, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
TYPE(MPI_Status) :: status
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_read_shared(fh, buf, count, datatype, status, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Status) :: status
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_seek(fh, offset, whence, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
INTEGER(KIND=MPI_OFFSET_KIND), INTENT(IN) :: offset 
INTEGER, INTENT(IN) :: whence 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_seek_shared(fh, offset, whence, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
INTEGER(KIND=MPI_OFFSET_KIND), INTENT(IN) :: offset 
INTEGER, INTENT(IN) :: whence 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_set_atomicity(fh, flag, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
LOGICAL, INTENT(IN) :: flag 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_set_info(fh, info, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
TYPE(MPI_Info), INTENT(IN) :: info 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_set_size(fh, size, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
INTEGER(KIND=MPI_OFFSET_KIND), INTENT(IN) :: size 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_set_view(fh, disp, etype, filetype, datarep, info, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
INTEGER(KIND=MPI_OFFSET_KIND), INTENT(IN) :: disp 
TYPE(MPI_Datatype), INTENT(IN) :: etype 
TYPE(MPI_Datatype), INTENT(IN) :: filetype 
CHARACTER(LEN=*), INTENT(IN) :: datarep 
TYPE(MPI_Info), INTENT(IN) :: info 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_sync(fh, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_write(fh, buf, count, datatype, status, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Status) :: status
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_write_all(fh, buf, count, datatype, status, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Status) :: status
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_write_all_begin(fh, buf, count, datatype, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_write_all_end(fh, buf, status, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
TYPE(MPI_Status) :: status
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_write_at(fh, offset, buf, count, datatype, status, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
INTEGER(KIND=MPI_OFFSET_KIND), INTENT(IN) :: offset 
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Status) :: status
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_write_at_all(fh, offset, buf, count, datatype, status, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
INTEGER(KIND=MPI_OFFSET_KIND), INTENT(IN) :: offset 
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Status) :: status
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_write_at_all_begin(fh, offset, buf, count, datatype, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
INTEGER(KIND=MPI_OFFSET_KIND), INTENT(IN) :: offset 
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_write_at_all_end(fh, buf, status, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
TYPE(MPI_Status) :: status
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_write_ordered(fh, buf, count, datatype, status, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Status) :: status
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_write_ordered_begin(fh, buf, count, datatype, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_write_ordered_end(fh, buf, status, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
TYPE(MPI_Status) :: status
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_File_write_shared(fh, buf, count, datatype, status, ierror) 
USE mpi_f08_types
implicit none
TYPE(MPI_File), INTENT(IN) :: fh 
!TYPE(*), dimension(..) :: buf 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: buf
!$PRAGMA IGNORE_TKR buf
REAL, DIMENSION(*) :: buf
INTEGER, INTENT(IN) :: count 
TYPE(MPI_Datatype), INTENT(IN) :: datatype 
TYPE(MPI_Status) :: status
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Register_datarep(datarep, read_conversion_fn, write_conversion_fn, dtype_file_extent_fn, extra_state, ierror) 
USE mpi_f08_types
implicit none
CHARACTER(LEN=*), INTENT(IN) :: datarep 
PROCEDURE() :: read_conversion_fn, write_conversion_fn, dtype_file_extent_fn 
INTEGER(KIND=MPI_ADDRESS_KIND), INTENT(IN) :: extra_state 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 
!
!A.4.12 Language Bindings Fortran Bindings
!
SUBROUTINE MPI_Sizeof(x, size, ierror) 
USE mpi_f08_types
implicit none
!TYPE(*) :: x 
!DEC$ ATTRIBUTES NO_ARG_CHECK :: x
!$PRAGMA IGNORE_TKR x
REAL, DIMENSION(*) :: x
INTEGER, INTENT(OUT) :: size 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Type_create_f90_complex(p, r, newtype, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: p, r 
TYPE(MPI_Datatype), INTENT(OUT) :: newtype 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Type_create_f90_integer(r, newtype, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: r 
TYPE(MPI_Datatype), INTENT(OUT) :: newtype 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Type_create_f90_real(p, r, newtype, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: p, r 
TYPE(MPI_Datatype), INTENT(OUT) :: newtype 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 

SUBROUTINE MPI_Type_match_size(typeclass, size, type, ierror) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: typeclass, size 
TYPE(MPI_Datatype), INTENT(OUT) :: type 
INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
END SUBROUTINE 
!(This routine specification may be changed by Ticket #252-W). 
!
!A.3.13 Profiling Interface Fortran Bindings
!
SUBROUTINE MPI_Pcontrol(level) 
USE mpi_f08_types
implicit none
INTEGER, INTENT(IN) :: level 
END SUBROUTINE 
!
!Impact on Implementations
!
!See previous tickets.
!
!Impact on Applications / Users
!
!See previous tickets.
!
!Alternative Solutions
!
!If one wants to implement also the currently deprecated functions with the new interface, then these interfaces are used:
!
!A.4.14 Deprecated Fortran Bindings
!
!SUBROUTINE MPI_Address(location, address, ierror) 
!USE mpi_f08_types
!TYPE(*), dimension(..) :: location 
!INTEGER, INTENT(OUT) :: address 
!INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
!END SUBROUTINE 
!
!SUBROUTINE MPI_Attr_delete(comm, keyval, ierror) 
!USE mpi_f08_types
!TYPE(MPI_Comm), INTENT(IN) :: comm 
!INTEGER, INTENT(IN) :: keyval 
!INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
!END SUBROUTINE 
!
!SUBROUTINE MPI_Attr_get(comm, keyval, attribute_val, flag, ierror) 
!USE mpi_f08_types
!TYPE(MPI_Comm), INTENT(IN) :: comm 
!INTEGER, INTENT(IN) :: keyval 
!INTEGER, INTENT(OUT) :: attribute_val 
!LOGICAL, INTENT(OUT) :: flag 
!INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
!END SUBROUTINE 
!
!SUBROUTINE MPI_Attr_put(comm, keyval, attribute_val, ierror) 
!USE mpi_f08_types
!TYPE(MPI_Comm), INTENT(IN) :: comm 
!INTEGER, INTENT(IN) :: keyval, attribute_val 
!INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
!END SUBROUTINE 
!
!SUBROUTINE MPI_DUP_FN(oldcomm, keyval, extra_state, attribute_val_in, attribute_val_out, flag, ierror) 
!USE mpi_f08_types
!TYPE(MPI_Comm), INTENT(IN) :: oldcomm 
!INTEGER, INTENT(IN) :: keyval, extra_state, attribute_val_in 
!INTEGER, INTENT(OUT) :: attribute_val_out 
!LOGICAL, INTENT(OUT) :: flag 
!INTEGER, INTENT(OUT) :: ierror 
!END SUBROUTINE 
!
!SUBROUTINE MPI_Errhandler_create(func, errhandler, ierror) 
!USE mpi_f08_types
!PROCEDURE() :: func
!TYPE(MPI_Errhandler), INTENT(OUT) :: errhandler 
!INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
!END SUBROUTINE 
!
!SUBROUTINE MPI_Errhandler_get(comm, errhandler, ierror) 
!USE mpi_f08_types
!TYPE(MPI_Comm), INTENT(IN) :: comm 
!TYPE(MPI_Errhandler), INTENT(OUT) :: errhandler 
!INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
!END SUBROUTINE 
!
!SUBROUTINE MPI_Errhandler_set(comm, errhandler, ierror) 
!USE mpi_f08_types
!TYPE(MPI_Comm), INTENT(IN) :: comm 
!TYPE(MPI_Errhandler), INTENT(IN) :: errhandler 
!INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
!END SUBROUTINE 
!
!SUBROUTINE MPI_Keyval_create(copy_fn, delete_fn, keyval, extra_state, ierror) 
!USE mpi_f08_types
!PROCEDURE() :: copy_fn, delete_fn 
!INTEGER, INTENT(OUT) :: keyval 
!INTEGER, INTENT(IN) :: extra_state 
!INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
!END SUBROUTINE 
!
!SUBROUTINE MPI_Keyval_free(keyval, ierror) 
!USE mpi_f08_types
!INTEGER, INTENT(INOUT) :: keyval 
!INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
!END SUBROUTINE 
!
!SUBROUTINE MPI_NULL_COPY_FN(oldcomm, keyval, extra_state, attribute_val_in, attribute_val_out, flag, ierror) 
!USE mpi_f08_types
!TYPE(MPI_Comm), INTENT(IN) :: oldcomm 
!INTEGER, INTENT(IN) :: keyval, extra_state, attribute_val_in 
!INTEGER, INTENT(OUT) :: attribute_val_out 
!LOGICAL, INTENT(OUT) :: flag 
!INTEGER, INTENT(OUT) :: ierror 
!END SUBROUTINE 
!
!SUBROUTINE MPI_NULL_DELETE_FN(comm, keyval, attribute_val, extra_state, ierror) 
!USE mpi_f08_types
!TYPE(MPI_Comm), INTENT(IN) :: comm 
!INTEGER, INTENT(IN) :: keyval, attribute_val, extra_state 
!INTEGER, INTENT(OUT) :: ierror 
!END SUBROUTINE 
!
!SUBROUTINE MPI_Type_extent(datatype, extent, ierror) 
!USE mpi_f08_types
!TYPE(MPI_Datatype), INTENT(IN) :: datatype 
!INTEGER, INTENT(OUT) :: extent 
!INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
!END SUBROUTINE 
!
!SUBROUTINE MPI_Type_hindexed(count, array_of_blocklengths, array_of_displacements, oldtype, newtype, ierror) 
!USE mpi_f08_types
!INTEGER, INTENT(IN) :: count, array_of_blocklengths(*), array_of_displacements(*) 
!TYPE(MPI_Datatype), INTENT(IN) :: oldtype 
!TYPE(MPI_Datatype), INTENT(OUT) :: newtype 
!INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
!END SUBROUTINE 
!
!SUBROUTINE MPI_Type_hvector(count, blocklength, stride, oldtype, newtype, ierror) 
!USE mpi_f08_types
!INTEGER, INTENT(IN) :: count, blocklength, stride 
!TYPE(MPI_Datatype), INTENT(IN) :: oldtype 
!TYPE(MPI_Datatype), INTENT(OUT) :: newtype 
!INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
!END SUBROUTINE 
!
!SUBROUTINE MPI_Type_lb( datatype, displacement, ierror) 
!USE mpi_f08_types
!TYPE(MPI_Datatype), INTENT(IN) :: datatype 
!INTEGER, INTENT(OUT) :: displacement 
!INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
!END SUBROUTINE 
!
!SUBROUTINE MPI_Type_struct(count, array_of_blocklengths, array_of_displacements, array_of_types, newtype, ierror) 
!USE mpi_f08_types
!INTEGER, INTENT(IN) :: count, array_of_blocklengths(*), array_of_displacements(*) 
!TYPE(MPI_Datatype), INTENT(IN) :: array_of_types(*) 
!TYPE(MPI_Datatype), INTENT(OUT) :: newtype 
!INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
!END SUBROUTINE 
!
!SUBROUTINE MPI_Type_ub( datatype, displacement, ierror) 
!USE mpi_f08_types
!TYPE(MPI_Datatype), INTENT(IN) :: datatype 
!INTEGER, INTENT(OUT) :: displacement 
!INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
!END SUBROUTINE 

end interface

END
