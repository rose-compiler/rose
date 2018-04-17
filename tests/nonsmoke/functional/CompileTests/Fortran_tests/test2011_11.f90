  type :: t
    integer :: d
  end type

!  type(t) :: q(100)

interface

 SUBROUTINE MPI_Startall(count, array_of_requests, ierror) 
! USE mpi_f08_types
  implicit none

 INTEGER, INTENT(IN) :: count 
 TYPE(t), INTENT(INOUT) :: array_of_requests(*) 
 INTEGER, OPTIONAL, INTENT(OUT) :: ierror 
 END SUBROUTINE 

end interface

end
