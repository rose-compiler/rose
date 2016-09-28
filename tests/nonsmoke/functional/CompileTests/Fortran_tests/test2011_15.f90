 subroutine create_boundary_simple( dist )

   type :: distrb
    ! integer proc(7)
      integer , dimension(:), pointer :: &
         proc              ,&! processor location for this block
         local_block         ! block position in local array on proc
   end type

   integer , dimension(:), pointer :: &
         proc              ,&! processor location for this block
         local_block         ! block position in local array on proc

! !INPUT PARAMETERS:
  type (distrb), intent(in) :: dist  ! distribution of blocks across procs

   integer :: nblocks

   nblocks = size(dist%proc(:))

 ! lalloc_tripole = .false.
 ! newbndy%communicator = dist%communicator

end subroutine
