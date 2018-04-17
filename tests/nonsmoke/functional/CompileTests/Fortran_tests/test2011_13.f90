 subroutine create_boundary_simple( dist )

   type :: distrb
    ! integer proc(7)
      integer , dimension(:), pointer :: &
         proc              ,&! processor location for this block
         local_block         ! block position in local array on proc
   end type

! !INPUT PARAMETERS:
   type (distrb), intent(in) :: dist  ! distribution of blocks across procs

   integer :: nblocks

   nblocks = size(dist%proc(:))

 ! lalloc_tripole = .false.
 ! newbndy%communicator = dist%communicator

end subroutine


module distribution

   implicit none
   private
   save

! !PUBLIC TYPES:

   type, public :: distrb  ! distribution data type
      integer :: &
         nprocs            ,&! number of processors in this dist
         communicator        ! communicator to use in this dist

      integer , dimension(:), pointer :: &
         proc              ,&! processor location for this block
         local_block         ! block position in local array on proc
   end type

contains 

subroutine create_boundary(newbndy, dist, &
                            ns_bndy_type, ew_bndy_type, &
                            nx_global, ny_global)

! !DESCRIPTION:
!  This routine creates a boundary type with info necessary for
!  performing a boundary (ghost cell) update based on the input block
!  distribution.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   integer  newbndy

   type (distrb), intent(in) :: &
      dist       ! distribution of blocks across procs

   character (*), intent(in) :: &
      ns_bndy_type,             &! type of boundary to use in ns dir
      ew_bndy_type               ! type of boundary to use in ew dir

   integer, intent(in) :: &
      nx_global, ny_global       ! global extents of domain

! !OUTPUT PARAMETERS:

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer ::           &
      i,j,k,n,                     &! dummy counters
      iblock_src  , jblock_src  ,  &! i,j index of source block
      iblock_dst  , jblock_dst  ,  &! i,j index of dest   block
      iblock_north, jblock_north,  &! i,j index of north neighbor block
      iblock_south, jblock_south,  &! i,j index of south neighbor block
      iblock_east , jblock_east ,  &! i,j index of east  neighbor block
      iblock_west , jblock_west ,  &! i,j index of west  neighbor block
      src_block_loc,               &! local block location of source
      dst_block_loc,               &! local block location of dest
      imsg_ew_snd, imsg_ew_rcv,    &! counters for ew send/recv
      imsg_ns_snd, imsg_ns_rcv,    &! counters for ns send/recv
      nprocs,                      &! num of processors involved
      nblocks,                     &! total number of blocks
      bid, pid,                    &! block and processor locators
      iblk, imsg,                  &!
      iloc_ew, iloc_ns,            &!
      src_proc, dst_proc            ! src,dst processor for message

   logical :: &
      lalloc_tripole      ! flag for allocating tripole buffers

   integer, dimension(:), allocatable :: &
      ew_snd_count,    &! array for counting blocks in each message
      ew_rcv_count,    &! array for counting blocks in each message
      ns_snd_count,    &! array for counting blocks in each message
      ns_rcv_count,    &! array for counting blocks in each message
      msg_ew_snd  ,    &! msg counter for each active processor
      msg_ew_rcv  ,    &! msg counter for each active processor
      msg_ns_snd  ,    &! msg counter for each active processor
      msg_ns_rcv        ! msg counter for each active processor

!-----------------------------------------------------------------------
!
!  Initialize some useful variables and return if this task not
!  in the current distribution.
!
!-----------------------------------------------------------------------

   nprocs = dist%nprocs

!   if (my_task >= nprocs) return

   nblocks = size(dist%proc(:))
   lalloc_tripole = .false.
!   newbndy%communicator = dist%communicator

end subroutine

end module

end

