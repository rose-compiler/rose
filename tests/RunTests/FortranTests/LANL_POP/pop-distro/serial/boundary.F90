!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
!BOP
! !MODULE: boundary

 module boundary

! !DESCRIPTION:
!  This module contains data types and routines for updating ghost cell
!  boundaries using MPI calls
!
! !REVISION HISTORY:
!  CVS:$Id: boundary.F90,v 1.12 2004/01/07 19:56:38 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $

! !USES:

   use kinds_mod
   use communicate
   use constants
   use blocks
   use distribution
   use exit_mod
   !use timers

   implicit none
   private
   save

! !PUBLIC TYPES:

   type, public :: bndy
     integer (int_kind) :: &
       communicator       ,&! communicator to use for update messages
       nlocal_ew          ,&! num local copies for east-west bndy update
       nlocal_ns            ! num local copies for east-west bndy update

     integer (int_kind), dimension(:), pointer :: &
       local_ew_src_block ,&! source block for each local east-west copy
       local_ew_dst_block ,&! dest   block for each local east-west copy
       local_ns_src_block ,&! source block for each local north-south copy
       local_ns_dst_block   ! dest   block for each local north-south copy

     integer (int_kind), dimension(:,:), pointer :: &
       local_ew_src_add   ,&! starting source address for local e-w copies
       local_ew_dst_add   ,&! starting dest   address for local e-w copies
       local_ns_src_add   ,&! starting source address for local n-s copies
       local_ns_dst_add     ! starting dest   address for local n-s copies

   end type bndy

! !PUBLIC MEMBER FUNCTIONS:

   public :: create_boundary,  &
             destroy_boundary, &
             update_ghost_cells

   interface update_ghost_cells  ! generic interface
     module procedure boundary_2d_dbl,  &
                      boundary_2d_real, &
                      boundary_2d_int,  &
                      boundary_3d_dbl,  &
                      boundary_3d_real, &
                      boundary_3d_int,  &
                      boundary_4d_dbl,  &
                      boundary_4d_real, &
                      boundary_4d_int
   end interface

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  global boundary buffers for tripole boundary
!
!-----------------------------------------------------------------------

   integer (int_kind), dimension(:,:), allocatable :: &
      tripole_ibuf,  &
      tripole_ighost

   real (r4), dimension(:,:), allocatable :: &
      tripole_rbuf,  &
      tripole_rghost

   real (r8), dimension(:,:), allocatable :: &
      tripole_dbuf,  &
      tripole_dghost

!EOC
!***********************************************************************

contains

!***********************************************************************
!BOP
! !IROUTINE: create_boundary
! !INTERFACE:

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

   type (distrb), intent(in) :: &
      dist       ! distribution of blocks across procs

   character (*), intent(in) :: &
      ns_bndy_type,             &! type of boundary to use in ns dir
      ew_bndy_type               ! type of boundary to use in ew dir

   integer (int_kind), intent(in) :: &
      nx_global, ny_global       ! global extents of domain

! !OUTPUT PARAMETERS:

   type (bndy), intent(out) :: &
      newbndy    ! a new boundary type with info for updates

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) ::           &
      i,j,k,n,                     &! dummy counters
      iblock_src  , jblock_src  ,  &! i,j index of source block
      iblock_dst  , jblock_dst  ,  &! i,j index of dest   block
      iblock_north, jblock_north,  &! i,j index of north neighbor block
      iblock_south, jblock_south,  &! i,j index of south neighbor block
      iblock_east , jblock_east ,  &! i,j index of east  neighbor block
      iblock_west , jblock_west ,  &! i,j index of west  neighbor block
      src_block_loc,               &! local block location of source
      dst_block_loc,               &! local block location of dest
      nprocs,                      &! num of processors involved
      nblocks,                     &! total number of blocks
      iloc_ew, iloc_ns,            &!
      src_proc, dst_proc            ! src,dst processor for message

   logical (log_kind) :: &
      lalloc_tripole      ! flag for allocating tripole buffers

   type (block) ::     &
      src_block,       &! block info for source      block
      dst_block         ! block info for destination block

!-----------------------------------------------------------------------
!
!  Initialize some useful variables and return if this task not
!  in the current distribution.
!
!-----------------------------------------------------------------------

   nprocs = dist%nprocs

   if (my_task >= nprocs) return

   nblocks = size(dist%proc(:))
   lalloc_tripole = .false.
   newbndy%communicator = dist%communicator

!-----------------------------------------------------------------------
!
!  Count the number of messages to send/recv from each processor
!  and number of blocks in each message.  These quantities are
!  necessary for allocating future arrays.
!
!-----------------------------------------------------------------------

   iloc_ew = 0
   iloc_ns = 0

   block_loop1: do n=1,nblocks
      src_proc  = dist%proc(n)
      src_block = get_block(n,n)

      iblock_src = src_block%iblock  ! i,j index of this block in
      jblock_src = src_block%jblock  !   block cartesian decomposition

      !*** compute cartesian i,j block indices for each neighbor
      !*** use zero if off the end of closed boundary
      !*** use jnorth=nblocks_y and inorth < 0 for tripole boundary
      !***   to make sure top boundary communicated to all top
      !***   boundary blocks

      select case(ew_bndy_type)
      case ('cyclic')
         iblock_east = mod(iblock_src,nblocks_x) + 1
         iblock_west = iblock_src - 1
         if (iblock_west == 0) iblock_west = nblocks_x
         jblock_east = jblock_src
         jblock_west = jblock_src
      case ('closed')
         iblock_east = iblock_src + 1
         iblock_west = iblock_src - 1
         if (iblock_east > nblocks_x) iblock_east = 0
         if (iblock_west < 1        ) iblock_west = 0
         jblock_east = jblock_src
         jblock_west = jblock_src
      case default
         call exit_POP(sigAbort, 'Unknown east-west boundary type')
      end select

      select case(ns_bndy_type)
      case ('cyclic')
         jblock_north = mod(jblock_src,nblocks_y) + 1
         jblock_south = jblock_src - 1
         if (jblock_south == 0) jblock_south = nblocks_y
         iblock_north = iblock_src
         iblock_south = iblock_src
      case ('closed')
         jblock_north = jblock_src + 1
         jblock_south = jblock_src - 1
         if (jblock_north > nblocks_y) jblock_north = 0
         if (jblock_south < 1        ) jblock_south = 0
         iblock_north = iblock_src
         iblock_south = iblock_src
      case ('tripole')
         lalloc_tripole = .true.
         jblock_north = jblock_src + 1
         jblock_south = jblock_src - 1
         iblock_north = iblock_src
         iblock_south = iblock_src
         if (jblock_south < 1        ) jblock_south = 0
         if (jblock_north > nblocks_y) then
            jblock_north = nblocks_y
            iblock_north = -iblock_src
         endif
      case default
         call exit_POP(sigAbort, 'Unknown north-south boundary type')
      end select

      !***
      !*** if any neighbors are closed boundaries, must
      !*** create a local pseudo-message to zero ghost cells
      !***

      if (src_proc /= 0) then
         if (iblock_east  == 0) iloc_ew = iloc_ew + 1
         if (iblock_west  == 0) iloc_ew = iloc_ew + 1
         if (jblock_north == 0) iloc_ns = iloc_ns + 1
         if (jblock_south == 0) iloc_ns = iloc_ns + 1
      endif

      !***
      !*** now look through all the blocks for the neighbors
      !*** of the source block and check whether a message is
      !*** required for communicating with the neighbor
      !***

      do k=1,nblocks
         dst_block = get_block(k,k)

         iblock_dst = dst_block%iblock  !*** i,j block index of
         jblock_dst = dst_block%jblock  !*** potential neighbor block

         dst_proc = dist%proc(k)  ! processor that holds dst block

         !***
         !*** if this block is an eastern neighbor
         !*** increment message counter
         !***

         if (iblock_dst == iblock_east .and. &
             jblock_dst == jblock_east) then

            if (dst_proc /= 0) then
               iloc_ew = iloc_ew + 1
            endif
         endif

         !***
         !*** if this block is an western neighbor
         !*** increment message counter
         !***

         if (iblock_dst == iblock_west .and. &
             jblock_dst == jblock_west) then

            if (dst_proc /= 0) then
               iloc_ew = iloc_ew + 1
            endif
         endif

         !***
         !*** if this block is an northern neighbor
         !*** find out whether a message is required
         !*** for tripole, must communicate with all
         !*** north row blocks (triggered by iblock_dst <0)
         !***

         if ((iblock_dst == iblock_north .or. iblock_north < 0) .and. &
              jblock_dst == jblock_north) then

            if (dst_proc /= 0) then
               iloc_ns = iloc_ns + 1
            endif
         endif

         !***
         !*** if this block is an southern neighbor
         !*** find out whether a message is required
         !***

         if (iblock_dst == iblock_south .and. &
             jblock_dst == jblock_south) then

            if (dst_proc /= 0) then
               iloc_ns = iloc_ns + 1
            endif
         endif

      end do  ! search for dest blocks
   end do block_loop1

   !***
   !*** in this serial version, all messages are local copies
   !***

   newbndy%nlocal_ew = iloc_ew
   newbndy%nlocal_ns = iloc_ns

!-----------------------------------------------------------------------
!
!  allocate buffers and arrays necessary for boundary comms
!
!-----------------------------------------------------------------------

   allocate (newbndy%local_ew_src_block(newbndy%nlocal_ew), &
             newbndy%local_ew_dst_block(newbndy%nlocal_ew), &
             newbndy%local_ns_src_block(newbndy%nlocal_ns), &
             newbndy%local_ns_dst_block(newbndy%nlocal_ns), &
             newbndy%local_ew_src_add(2,newbndy%nlocal_ew), &
             newbndy%local_ew_dst_add(2,newbndy%nlocal_ew), &
             newbndy%local_ns_src_add(2,newbndy%nlocal_ns), &
             newbndy%local_ns_dst_add(2,newbndy%nlocal_ns))

   newbndy%local_ew_src_block = 0
   newbndy%local_ew_dst_block = 0
   newbndy%local_ns_src_block = 0
   newbndy%local_ns_dst_block = 0
   newbndy%local_ew_src_add = 0
   newbndy%local_ew_dst_add = 0
   newbndy%local_ns_src_add = 0
   newbndy%local_ns_dst_add = 0

!-----------------------------------------------------------------------
!
!  now set up indices into buffers and address arrays
!
!-----------------------------------------------------------------------

   iloc_ew = 0
   iloc_ns = 0

!-----------------------------------------------------------------------
!
!  repeat loop through blocks but this time, determine all the
!  required message information for each message or local copy
!
!-----------------------------------------------------------------------

   block_loop2: do n=1,nblocks

      src_proc  = dist%proc(n)    ! processor location for this block
      src_block = get_block(n,n)  ! block info for this block

      iblock_src = src_block%iblock  ! i,j index of this block in
      jblock_src = src_block%jblock  !   block cartesian decomposition

      if (src_proc /= 0) then
         src_block_loc = dist%local_block(n)  ! local block location
      else
         src_block_loc = 0  ! block is a land block
      endif

      !*** compute cartesian i,j block indices for each neighbor
      !*** use zero if off the end of closed boundary
      !*** use jnorth=nblocks_y and inorth < 0 for tripole boundary
      !***   to make sure top boundary communicated to all top
      !***   boundary blocks

      select case(ew_bndy_type)
      case ('cyclic')
         iblock_east = mod(iblock_src,nblocks_x) + 1
         iblock_west = iblock_src - 1
         if (iblock_west == 0) iblock_west = nblocks_x
         jblock_east = jblock_src
         jblock_west = jblock_src
      case ('closed')
         iblock_east = iblock_src + 1
         iblock_west = iblock_src - 1
         if (iblock_east > nblocks_x) iblock_east = 0
         if (iblock_west < 1        ) iblock_west = 0
         jblock_east = jblock_src
         jblock_west = jblock_src
      case default
         call exit_POP(sigAbort, 'Unknown east-west boundary type')
      end select

      select case(ns_bndy_type)
      case ('cyclic')
         jblock_north = mod(jblock_src,nblocks_y) + 1
         jblock_south = jblock_src - 1
         if (jblock_south == 0) jblock_south = nblocks_y
         iblock_north = iblock_src
         iblock_south = iblock_src
      case ('closed')
         jblock_north = jblock_src + 1
         jblock_south = jblock_src - 1
         if (jblock_north > nblocks_y) jblock_north = 0
         if (jblock_south < 1        ) jblock_south = 0
         iblock_north = iblock_src
         iblock_south = iblock_src
      case ('tripole')
         jblock_north = jblock_src + 1
         jblock_south = jblock_src - 1
         iblock_north = iblock_src
         iblock_south = iblock_src
         if (jblock_south < 1        ) jblock_south = 0
         if (jblock_north > nblocks_y) then
            jblock_north = nblocks_y
            iblock_north = -iblock_src
         endif
      case default
         call exit_POP(sigAbort, 'Unknown north-south boundary type')
      end select

      !***
      !*** if any boundaries are closed boundaries, set up
      !*** pseudo-message to zero ghost cells
      !***

      if (src_block_loc /= 0) then
         if (iblock_east == 0) then
            iloc_ew = iloc_ew + 1
            newbndy%local_ew_src_block(iloc_ew) = 0
            newbndy%local_ew_src_add(1,iloc_ew) = 0
            newbndy%local_ew_src_add(2,iloc_ew) = 0
            newbndy%local_ew_dst_block(iloc_ew) = src_block_loc
            newbndy%local_ew_dst_add(1,iloc_ew) = src_block%ie + 1
            newbndy%local_ew_dst_add(2,iloc_ew) = 1
         else if (iblock_west == 0) then
            iloc_ew = iloc_ew + 1
            newbndy%local_ew_src_block(iloc_ew) = 0
            newbndy%local_ew_src_add(1,iloc_ew) = 0
            newbndy%local_ew_src_add(2,iloc_ew) = 0
            newbndy%local_ew_dst_block(iloc_ew) = src_block_loc
            newbndy%local_ew_dst_add(1,iloc_ew) = 1
            newbndy%local_ew_dst_add(2,iloc_ew) = 1
         else if (jblock_north == 0) then
            iloc_ns = iloc_ns + 1
            newbndy%local_ns_src_block(iloc_ns) = 0
            newbndy%local_ns_src_add(1,iloc_ns) = 0
            newbndy%local_ns_src_add(2,iloc_ns) = 0
            newbndy%local_ns_dst_block(iloc_ns) = src_block_loc
            newbndy%local_ns_dst_add(1,iloc_ns) = 1
            newbndy%local_ns_dst_add(2,iloc_ns) = src_block%je + 1
         else if (jblock_south == 0) then
            iloc_ns = iloc_ns + 1
            newbndy%local_ns_src_block(iloc_ns) = 0
            newbndy%local_ns_src_add(1,iloc_ns) = 0
            newbndy%local_ns_src_add(2,iloc_ns) = 0
            newbndy%local_ns_dst_block(iloc_ns) = src_block_loc
            newbndy%local_ns_dst_add(1,iloc_ns) = 1
            newbndy%local_ns_dst_add(2,iloc_ns) = 1
         endif
      endif

      !***
      !*** now search through blocks looking for neighbors to
      !*** the source block
      !***

      do k=1,nblocks

         dst_proc      = dist%proc(k)  ! processor holding dst block

         !***
         !*** compute the rest only if this block is not a land block
         !***

         if (dst_proc /= 0) then

            dst_block = get_block(k,k)  ! block info for this block

            iblock_dst = dst_block%iblock  ! i,j block index in 
            jblock_dst = dst_block%jblock  ! Cartesian block decomposition

            dst_block_loc = dist%local_block(k)  ! local block location

            !***
            !*** if this block is an eastern neighbor
            !*** determine send/receive addresses
            !***

            if (iblock_dst == iblock_east .and. &
                jblock_dst == jblock_east) then

               if (src_proc /= 0) then
                  !*** local copy from one block to another
                  iloc_ew = iloc_ew + 1
                  newbndy%local_ew_src_block(iloc_ew) = src_block_loc
                  newbndy%local_ew_src_add(1,iloc_ew) = src_block%ie - &
                                                        nghost + 1
                  newbndy%local_ew_src_add(2,iloc_ew) = 1
                  newbndy%local_ew_dst_block(iloc_ew) = dst_block_loc
                  newbndy%local_ew_dst_add(1,iloc_ew) = 1
                  newbndy%local_ew_dst_add(2,iloc_ew) = 1
               else
                  !*** source block is all land so treat as local copy
                  !*** with source block zero to fill ghost cells with 
                  !*** zeroes
                  iloc_ew = iloc_ew + 1
                  newbndy%local_ew_src_block(iloc_ew) = 0
                  newbndy%local_ew_src_add(1,iloc_ew) = 0
                  newbndy%local_ew_src_add(2,iloc_ew) = 0
                  newbndy%local_ew_dst_block(iloc_ew) = dst_block_loc
                  newbndy%local_ew_dst_add(1,iloc_ew) = 1
                  newbndy%local_ew_dst_add(2,iloc_ew) = 1
               endif

            endif ! east neighbor

            !***
            !*** if this block is a western neighbor
            !*** determine send/receive addresses
            !***

            if (iblock_dst == iblock_west .and. &
                jblock_dst == jblock_west) then

               if (src_proc /= 0) then
                  !*** perform a local copy
                  iloc_ew = iloc_ew + 1
                  newbndy%local_ew_src_block(iloc_ew) = src_block_loc
                  newbndy%local_ew_src_add(1,iloc_ew) = nghost + 1
                  newbndy%local_ew_src_add(2,iloc_ew) = 1
                  newbndy%local_ew_dst_block(iloc_ew) = dst_block_loc
                  newbndy%local_ew_dst_add(1,iloc_ew) = dst_block%ie + 1
                  newbndy%local_ew_dst_add(2,iloc_ew) = 1
               else
                  !*** neighbor is a land block so zero ghost cells
                  iloc_ew = iloc_ew + 1
                  newbndy%local_ew_src_block(iloc_ew) = 0
                  newbndy%local_ew_src_add(1,iloc_ew) = 0
                  newbndy%local_ew_src_add(2,iloc_ew) = 0
                  newbndy%local_ew_dst_block(iloc_ew) = dst_block_loc
                  newbndy%local_ew_dst_add(1,iloc_ew) = dst_block%ie + 1
                  newbndy%local_ew_dst_add(2,iloc_ew) = 1
               endif

            endif ! west neighbor

            !***
            !*** if this block is a northern neighbor
            !***  compute send/recv addresses
            !*** for tripole, must communicate with all
            !*** north row blocks (triggered by iblock_north <0)
            !***

            if ((iblock_dst == iblock_north .or. iblock_north < 0) .and. &
                 jblock_dst == jblock_north) then

               if (src_proc /= 0) then
                  !*** local copy
                  iloc_ns = iloc_ns + 1
                  newbndy%local_ns_src_block(iloc_ns) = src_block_loc
                  newbndy%local_ns_src_add(1,iloc_ns) = 1
                  newbndy%local_ns_src_add(2,iloc_ns) = src_block%je - &
                                                        nghost + 1
                  newbndy%local_ns_dst_block(iloc_ns) = dst_block_loc
                  newbndy%local_ns_dst_add(1,iloc_ns) = 1
                  newbndy%local_ns_dst_add(2,iloc_ns) = 1

                  if (iblock_north < 0) then !*** tripole boundary

                     newbndy%local_ns_dst_block(iloc_ns) = -dst_block_loc
                     !*** copy nghost+1 northern rows of physical
                     !*** domain into global north tripole buffer
                     newbndy%local_ns_src_add(1,iloc_ns) = &
                                        src_block%i_glob(nghost+1)
                     newbndy%local_ns_src_add(2,iloc_ns) = &
                                        dst_block%je - nghost

                     !*** copy out of tripole ghost cell buffer
                     !*** over-write the last row of the destination
                     !*** block to enforce for symmetry for fields
                     !*** located on domain boundary
                     newbndy%local_ns_dst_add(1,iloc_ns) = &
                                          dst_block%i_glob(nghost+1)
                     newbndy%local_ns_dst_add(2,iloc_ns) = & 
                                          dst_block%je
                  endif
               else
                  !*** source is land block so zero ghost cells
                  iloc_ns = iloc_ns + 1
                  newbndy%local_ns_src_block(iloc_ns) = 0
                  newbndy%local_ns_src_add(1,iloc_ns) = 0
                  newbndy%local_ns_src_add(2,iloc_ns) = 0
                  newbndy%local_ns_dst_block(iloc_ns) = dst_block_loc
                  newbndy%local_ns_dst_add(1,iloc_ns) = 1
                  newbndy%local_ns_dst_add(2,iloc_ns) = 1
                  if (iblock_north < 0) then !*** tripole boundary
                     newbndy%local_ns_dst_block(iloc_ns) = -dst_block_loc
                     !*** replace i addresses with global i location
                     !*** for copies into and out of global buffer
                     newbndy%local_ns_dst_add(1,iloc_ns) = &
                                             dst_block%i_glob(nghost+1)
                     newbndy%local_ns_dst_add(2,iloc_ns) = dst_block%je
                  endif
               endif

            endif ! north neighbor

            !***
            !*** if this block is a southern neighbor
            !*** determine send/receive addresses
            !***

            if (iblock_dst == iblock_south .and. &
                jblock_dst == jblock_south) then

               if (src_proc /= 0) then
                  !*** local copy
                  iloc_ns = iloc_ns + 1
                  newbndy%local_ns_src_block(iloc_ns) = src_block_loc
                  newbndy%local_ns_src_add(1,iloc_ns) = 1
                  newbndy%local_ns_src_add(2,iloc_ns) = nghost + 1
                  newbndy%local_ns_dst_block(iloc_ns) = dst_block_loc
                  newbndy%local_ns_dst_add(1,iloc_ns) = 1
                  newbndy%local_ns_dst_add(2,iloc_ns) = dst_block%je + 1
               else
                  !*** neighbor is a land block so zero ghost cells
                  iloc_ns = iloc_ns + 1
                  newbndy%local_ns_src_block(iloc_ns) = 0
                  newbndy%local_ns_src_add(1,iloc_ns) = 0
                  newbndy%local_ns_src_add(2,iloc_ns) = 0
                  newbndy%local_ns_dst_block(iloc_ns) = dst_block_loc
                  newbndy%local_ns_dst_add(1,iloc_ns) = 1
                  newbndy%local_ns_dst_add(2,iloc_ns) = dst_block%je + 1
               endif
            endif ! south neighbor

         endif  ! not a land block

      end do
   end do block_loop2

!-----------------------------------------------------------------------
!
!  if necessary, create tripole boundary buffers for each
!  common data type.  the ghost cell buffer includes an
!  extra row for the last physical row in order to enforce
!  symmetry conditions on variables at U points.  the other buffer
!  contains an extra row for handling y-offset for north face or
!  northeast corner points.
!
!-----------------------------------------------------------------------

   if (lalloc_tripole .and. .not. allocated(tripole_ibuf)) then
      allocate(tripole_ibuf  (nx_global,nghost+1), &
               tripole_rbuf  (nx_global,nghost+1), &
               tripole_dbuf  (nx_global,nghost+1), &
               tripole_ighost(nx_global,nghost+1), &
               tripole_rghost(nx_global,nghost+1), &
               tripole_dghost(nx_global,nghost+1))
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine create_boundary

!***********************************************************************
!BOP
! !IROUTINE: destroy_boundary
! !INTERFACE:

 subroutine destroy_boundary(in_bndy)

! !DESCRIPTION:
!  This routine destroys a boundary by deallocating all memory
!  associated with the boundary and nullifying pointers.
!
! !REVISION HISTORY:
!  same as module

! !INPUT/OUTPUT PARAMETERS:

   type (bndy), intent(inout) :: &
     in_bndy          ! boundary structure to be destroyed

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  reset all scalars
!
!-----------------------------------------------------------------------

   in_bndy%communicator      = 0
   in_bndy%nlocal_ew         = 0
   in_bndy%nlocal_ns         = 0

!-----------------------------------------------------------------------
!
!  deallocate all pointers
!
!-----------------------------------------------------------------------

   deallocate(in_bndy%local_ew_src_block, &
              in_bndy%local_ew_dst_block, &
              in_bndy%local_ns_src_block, &
              in_bndy%local_ns_dst_block, &
              in_bndy%local_ew_src_add,   &
              in_bndy%local_ew_dst_add,   &
              in_bndy%local_ns_src_add,   &
              in_bndy%local_ns_dst_add)

!-----------------------------------------------------------------------
!EOC

 end subroutine destroy_boundary

!***********************************************************************
!BOP
! !IROUTINE: update_ghost_cells
! !INTERFACE:

 subroutine boundary_2d_dbl(ARRAY, in_bndy, grid_loc, field_type)

! !DESCRIPTION:
!  This routine updates ghost cells for an input array and is a
!  member of a group of routines under the generic interface
!  update\_ghost\_cells.  This routine is the specific interface
!  for 2d horizontal arrays of double precision.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   type (bndy), intent(in) :: &
      in_bndy                 ! boundary update structure for the array

   integer (int_kind), intent(in) :: &
      field_type,               &! id for type of field (scalar, vector, angle)
      grid_loc                   ! id for location on horizontal grid
                                 !  (center, NEcorner, Nface, Eface)

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(:,:,:), intent(inout) :: &
      ARRAY              ! array containing horizontal slab to update

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) ::           &
      i,j,k,m,n,                   &! dummy loop indices
      ib_src,ie_src,jb_src,je_src, &! beg,end indices for bndy cells
      ib_dst,ie_dst,jb_dst,je_dst, &!
      nx_global,                   &! global domain size in x
      src_block,                   &! local block number for source
      dst_block,                   &! local block number for destination
      xoffset, yoffset,            &! address shifts for tripole
      isign                         ! sign factor for tripole grids

   real (r8) :: &
      xavg               ! scalar for enforcing symmetry at U pts

!-----------------------------------------------------------------------
!
!  set nx_global for tripole
!
!-----------------------------------------------------------------------

   if (allocated(tripole_dbuf)) nx_global = size(tripole_dbuf,dim=1)

!-----------------------------------------------------------------------
!
!  do local copies for east-west ghost cell updates
!  also initialize ghost cells to zero
!
!-----------------------------------------------------------------------

   !call timer_start(bndy_2d_local)
   do n=1,in_bndy%nlocal_ew
      src_block = in_bndy%local_ew_src_block(n)
      dst_block = in_bndy%local_ew_dst_block(n)

      ib_src = in_bndy%local_ew_src_add(1,n)
      ie_src = ib_src + nghost - 1
      ib_dst = in_bndy%local_ew_dst_add(1,n)
      ie_dst = ib_dst + nghost - 1

      if (src_block /= 0) then
         ARRAY(ib_dst:ie_dst,:,dst_block) = &
         ARRAY(ib_src:ie_src,:,src_block)
      else
         ARRAY(ib_dst:ie_dst,:,dst_block) = c0
      endif
   end do

!-----------------------------------------------------------------------
!
!  now exchange north-south boundary info
!
!-----------------------------------------------------------------------

   if (allocated(tripole_dbuf)) tripole_dbuf = c0

   do n=1,in_bndy%nlocal_ns
      src_block = in_bndy%local_ns_src_block(n)
      dst_block = in_bndy%local_ns_dst_block(n)

      if (dst_block > 0) then ! straight local copy

         jb_src = in_bndy%local_ns_src_add(2,n)
         je_src = jb_src + nghost - 1
         jb_dst = in_bndy%local_ns_dst_add(2,n)
         je_dst = jb_dst + nghost - 1

         if (src_block /= 0) then
            ARRAY(:,jb_dst:je_dst,dst_block) = &
            ARRAY(:,jb_src:je_src,src_block)
         else
            ARRAY(:,jb_dst:je_dst,dst_block) = c0
         endif

      else  !north boundary tripole grid - copy into global north buffer

         jb_src = in_bndy%local_ns_src_add(2,n)
         je_src = jb_src + nghost ! need nghost+1 rows for tripole

         !*** determine start, end addresses of physical domain
         !*** for both global buffer and local block

         ib_dst = in_bndy%local_ns_src_add(1,n)
         ie_dst = ib_dst + (nx_block-2*nghost) - 1
         if (ie_dst > nx_global) ie_dst = nx_global
         ib_src = nghost + 1
         ie_src = ib_src + ie_dst - ib_dst
         if (src_block /= 0) then
            tripole_dbuf(ib_dst:ie_dst,:) = &
                  ARRAY(ib_src:ie_src,jb_src:je_src,src_block)
         endif

      endif
   end do

!-----------------------------------------------------------------------
!
!  take care of northern boundary in tripole case
!
!-----------------------------------------------------------------------

   if (allocated(tripole_dbuf)) then

      select case (grid_loc)
      case (field_loc_center)   ! cell center location
         xoffset = 1
         yoffset = 1
         !*** first row of ghost cell buffer is actually the last
         !*** row of physical domain (mostly for symmetry enforcement)
         tripole_dghost(:,1) = tripole_dbuf(:,nghost+1)
      case (field_loc_NEcorner)   ! cell corner (velocity) location
         xoffset = 0
         yoffset = 0
         !*** enforce symmetry
         !*** first row of ghost cell buffer is actually the last
         !*** row of physical domain
         do i = 1, nx_global/2
            ib_dst = nx_global - i
            xavg = p5*(abs(tripole_dbuf(i     ,nghost+1)) + &
                       abs(tripole_dbuf(ib_dst,nghost+1)))
            tripole_dghost(i     ,1) = sign(xavg, &
                                            tripole_dbuf(i,nghost+1))
            tripole_dghost(ib_dst,1) = sign(xavg, &
                                            tripole_dbuf(ib_dst,nghost+1))
         end do
         !*** catch nx_global case (should be land anyway...)
         tripole_dghost(nx_global,1) = tripole_dbuf(nx_global,nghost+1)
         tripole_dbuf(:,nghost+1) = tripole_dghost(:,1)
      case (field_loc_Eface)   ! cell center location
         xoffset = 0
         yoffset = 1
         !*** first row of ghost cell buffer is actually the last
         !*** row of physical domain (mostly for symmetry enforcement)
         tripole_dghost(:,1) = tripole_dbuf(:,nghost+1)
      case (field_loc_Nface)   ! cell corner (velocity) location
         xoffset = 1
         yoffset = 0
         !*** enforce symmetry
         !*** first row of ghost cell buffer is actually the last
         !*** row of physical domain
         do i = 1, nx_global/2
            ib_dst = nx_global + 1 - i
            xavg = p5*(abs(tripole_dbuf(i     ,nghost+1)) + &
                       abs(tripole_dbuf(ib_dst,nghost+1)))
            tripole_dghost(i     ,1) = sign(xavg, &
                                            tripole_dbuf(i,nghost+1))
            tripole_dghost(ib_dst,1) = sign(xavg, &
                                            tripole_dbuf(ib_dst,nghost+1))
         end do
         tripole_dbuf(:,nghost+1) = tripole_dghost(:,1)
      case default
         call exit_POP(sigAbort, 'Unknown location in boundary_2d')
      end select

      select case (field_type)
      case (field_type_scalar)
         isign =  1
      case (field_type_vector)
         isign = -1
      case (field_type_angle)
         isign = -1
      case default
         call exit_POP(sigAbort, 'Unknown field type in boundary')
      end select

      !*** copy source (physical) cells into ghost cells
      !*** global source addresses are:
      !*** nx_global + xoffset - i
      !*** ny_global + yoffset - j
      !*** in the actual tripole buffer, the indices are:
      !*** nx_global + xoffset - i = ib_src - i
      !*** ny_global + yoffset - j - (ny_global - nghost) + 1 =
      !***    nghost + yoffset +1 - j = jb_src - j

      ib_src = nx_global + xoffset
      jb_src = nghost + yoffset + 1

      do j=1,nghost
      do i=1,nx_global
         tripole_dghost(i,1+j) = isign* &
                                 tripole_dbuf(ib_src-i, jb_src-j)
      end do
      end do

      !*** copy out of global ghost cell buffer into local
      !*** ghost cells

      do n=1,in_bndy%nlocal_ns
         dst_block = in_bndy%local_ns_dst_block(n)

         if (dst_block < 0) then
            dst_block = -dst_block

            jb_dst = in_bndy%local_ns_dst_add(2,n)
            je_dst = jb_dst + nghost
            ib_src = in_bndy%local_ns_dst_add(1,n)
            !*** ib_src is glob address of 1st point in physical
            !*** domain.  must now adjust to properly copy
            !*** east-west ghost cell info in the north boundary

            if (ib_src == 1) then  ! western boundary
               !*** impose cyclic conditions at western boundary
               !*** then set up remaining indices to copy rest
               !*** of domain from tripole ghost cell buffer
               do i=1,nghost
                  ARRAY(i,jb_dst:je_dst,dst_block) = &
                     tripole_dghost(nx_global-nghost+i,:)
               end do
               ie_src = ib_src + nx_block - nghost - 1
               if (ie_src > nx_global) ie_src = nx_global
               ib_dst = nghost + 1
               ie_dst = ib_dst + (ie_src - ib_src)
            else
               ib_src = ib_src - nghost
               ie_src = ib_src + nx_block - 1
               if (ie_src > nx_global) ie_src = nx_global
               ib_dst = 1
               ie_dst = ib_dst + (ie_src - ib_src)
            endif
            if (ie_src == nx_global) then ! eastern boundary
               !*** impose cyclic conditions in ghost cells
               do i=1,nghost
                  ARRAY(ie_dst+i,jb_dst:je_dst,dst_block) = &
                     tripole_dghost(i,:)
               end do
            endif

            !*** now copy the remaining ghost cell values

            ARRAY(ib_dst:ie_dst,jb_dst:je_dst,dst_block) = &
               tripole_dghost(ib_src:ie_src,:)
         endif

      end do

   endif

!-----------------------------------------------------------------------

 end subroutine boundary_2d_dbl

!***********************************************************************
!BOP
! !IROUTINE: update_ghost_cells
! !INTERFACE:

 subroutine boundary_2d_real(ARRAY, in_bndy, grid_loc, field_type)

! !DESCRIPTION:
!  This routine updates ghost cells for an input array and is a
!  member of a group of routines under the generic interface
!  update\_ghost\_cells.  This routine is the specific interface
!  for 2d horizontal arrays of single precision.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   type (bndy), intent(in) :: &
      in_bndy                 ! boundary update structure for the array

   integer (int_kind), intent(in) :: &
      field_type,               &! id for type of field (scalar, vector, angle)
      grid_loc                   ! id for location on horizontal grid
                                 !  (center, NEcorner, Nface, Eface)

! !INPUT/OUTPUT PARAMETERS:

   real (r4), dimension(:,:,:), intent(inout) :: &
      ARRAY              ! array containing horizontal slab to update

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) ::           &
      i,j,k,m,n,                   &! dummy loop indices
      ib_src,ie_src,jb_src,je_src, &! beg,end indices for bndy cells
      ib_dst,ie_dst,jb_dst,je_dst, &!
      nx_global,                   &! global domain size in x
      src_block,                   &! local block number for source
      dst_block,                   &! local block number for destination
      xoffset, yoffset,            &! address shifts for tripole
      isign                         ! sign factor for tripole grids

   real (r4) :: &
      xavg               ! scalar for enforcing symmetry at U pts

!-----------------------------------------------------------------------
!
!  set nx_global for tripole
!
!-----------------------------------------------------------------------

   if (allocated(tripole_rbuf)) nx_global = size(tripole_rbuf,dim=1)

!-----------------------------------------------------------------------
!
!  do local copies for east-west ghost cell updates
!  also initialize ghost cells to zero
!
!-----------------------------------------------------------------------

   do n=1,in_bndy%nlocal_ew
      src_block = in_bndy%local_ew_src_block(n)
      dst_block = in_bndy%local_ew_dst_block(n)

      ib_src = in_bndy%local_ew_src_add(1,n)
      ie_src = ib_src + nghost - 1
      ib_dst = in_bndy%local_ew_dst_add(1,n)
      ie_dst = ib_dst + nghost - 1

      if (src_block /= 0) then
         ARRAY(ib_dst:ie_dst,:,dst_block) = &
         ARRAY(ib_src:ie_src,:,src_block)
      else
         ARRAY(ib_dst:ie_dst,:,dst_block) = c0
      endif
   end do

!-----------------------------------------------------------------------
!
!  now exchange north-south boundary info
!
!-----------------------------------------------------------------------

   if (allocated(tripole_rbuf)) tripole_rbuf = c0

   do n=1,in_bndy%nlocal_ns
      src_block = in_bndy%local_ns_src_block(n)
      dst_block = in_bndy%local_ns_dst_block(n)

      if (dst_block > 0) then ! straight local copy

         jb_src = in_bndy%local_ns_src_add(2,n)
         je_src = jb_src + nghost - 1
         jb_dst = in_bndy%local_ns_dst_add(2,n)
         je_dst = jb_dst + nghost - 1

         if (src_block /= 0) then
            ARRAY(:,jb_dst:je_dst,dst_block) = &
            ARRAY(:,jb_src:je_src,src_block)
         else
            ARRAY(:,jb_dst:je_dst,dst_block) = c0
         endif

      else  !north boundary tripole grid - copy into global north buffer

         jb_src = in_bndy%local_ns_src_add(2,n)
         je_src = jb_src + nghost ! need nghost+1 rows for tripole

         !*** determine start, end addresses of physical domain
         !*** for both global buffer and local block

         ib_dst = in_bndy%local_ns_src_add(1,n)
         ie_dst = ib_dst + (nx_block-2*nghost) - 1
         if (ie_dst > nx_global) ie_dst = nx_global
         ib_src = nghost + 1
         ie_src = ib_src + ie_dst - ib_dst
         if (src_block /= 0) then
            tripole_rbuf(ib_dst:ie_dst,:) = &
                  ARRAY(ib_src:ie_src,jb_src:je_src,src_block)
         endif

      endif
   end do

!-----------------------------------------------------------------------
!
!  take care of northern boundary in tripole case
!
!-----------------------------------------------------------------------

   if (allocated(tripole_rbuf)) then

      select case (grid_loc)
      case (field_loc_center)   ! cell center location
         xoffset = 1
         yoffset = 1
         !*** first row of ghost cell buffer is actually the last
         !*** row of physical domain (mostly for symmetry enforcement)
         tripole_rghost(:,1) = tripole_rbuf(:,nghost+1)
      case (field_loc_NEcorner)   ! cell corner (velocity) location
         xoffset = 0
         yoffset = 0
         !*** enforce symmetry
         !*** first row of ghost cell buffer is actually the last
         !*** row of physical domain
         do i = 1, nx_global/2
            ib_dst = nx_global - i
            xavg = p5*(abs(tripole_rbuf(i     ,nghost+1)) + &
                       abs(tripole_rbuf(ib_dst,nghost+1)))
            tripole_rghost(i     ,1) = sign(xavg, &
                                            tripole_rbuf(i,nghost+1))
            tripole_rghost(ib_dst,1) = sign(xavg, &
                                            tripole_rbuf(ib_dst,nghost+1))
         end do
         !*** catch nx_global case (should be land anyway...)
         tripole_rghost(nx_global,1) = tripole_rbuf(nx_global,nghost+1)
         tripole_rbuf(:,nghost+1) = tripole_rghost(:,1)
      case (field_loc_Eface)   ! cell center location
         xoffset = 0
         yoffset = 1
         !*** first row of ghost cell buffer is actually the last
         !*** row of physical domain (mostly for symmetry enforcement)
         tripole_rghost(:,1) = tripole_rbuf(:,nghost+1)
      case (field_loc_Nface)   ! cell corner (velocity) location
         xoffset = 1
         yoffset = 0
         !*** enforce symmetry
         !*** first row of ghost cell buffer is actually the last
         !*** row of physical domain
         do i = 1, nx_global/2
            ib_dst = nx_global + 1 - i
            xavg = p5*(abs(tripole_rbuf(i     ,nghost+1)) + &
                       abs(tripole_rbuf(ib_dst,nghost+1)))
            tripole_rghost(i     ,1) = sign(xavg, &
                                            tripole_rbuf(i,nghost+1))
            tripole_rghost(ib_dst,1) = sign(xavg, &
                                            tripole_rbuf(ib_dst,nghost+1))
         end do
         tripole_rbuf(:,nghost+1) = tripole_rghost(:,1)
      case default
         call exit_POP(sigAbort, 'Unknown location in boundary_2d')
      end select

      select case (field_type)
      case (field_type_scalar)
         isign =  1
      case (field_type_vector)
         isign = -1
      case (field_type_angle)
         isign = -1
      case default
         call exit_POP(sigAbort, 'Unknown field type in boundary')
      end select

      !*** copy source (physical) cells into ghost cells
      !*** global source addresses are:
      !*** nx_global + xoffset - i
      !*** ny_global + yoffset - j
      !*** in the actual tripole buffer, the indices are:
      !*** nx_global + xoffset - i = ib_src - i
      !*** ny_global + yoffset - j - (ny_global - nghost) + 1 =
      !***    nghost + yoffset +1 - j = jb_src - j

      ib_src = nx_global + xoffset
      jb_src = nghost + yoffset + 1

      do j=1,nghost
      do i=1,nx_global
         tripole_rghost(i,1+j) = isign* &
                                 tripole_rbuf(ib_src-i, jb_src-j)
      end do
      end do

      !*** copy out of global ghost cell buffer into local
      !*** ghost cells

      do n=1,in_bndy%nlocal_ns
         dst_block = in_bndy%local_ns_dst_block(n)

         if (dst_block < 0) then
            dst_block = -dst_block

            jb_dst = in_bndy%local_ns_dst_add(2,n)
            je_dst = jb_dst + nghost
            ib_src = in_bndy%local_ns_dst_add(1,n)
            !*** ib_src is glob address of 1st point in physical
            !*** domain.  must now adjust to properly copy
            !*** east-west ghost cell info in the north boundary

            if (ib_src == 1) then  ! western boundary
               !*** impose cyclic conditions at western boundary
               !*** then set up remaining indices to copy rest
               !*** of domain from tripole ghost cell buffer
               do i=1,nghost
                  ARRAY(i,jb_dst:je_dst,dst_block) = &
                     tripole_rghost(nx_global-nghost+i,:)
               end do
               ie_src = ib_src + nx_block - nghost - 1
               if (ie_src > nx_global) ie_src = nx_global
               ib_dst = nghost + 1
               ie_dst = ib_dst + (ie_src - ib_src)
            else
               ib_src = ib_src - nghost
               ie_src = ib_src + nx_block - 1
               if (ie_src > nx_global) ie_src = nx_global
               ib_dst = 1
               ie_dst = ib_dst + (ie_src - ib_src)
            endif
            if (ie_src == nx_global) then ! eastern boundary
               !*** impose cyclic conditions in ghost cells
               do i=1,nghost
                  ARRAY(ie_dst+i,jb_dst:je_dst,dst_block) = &
                     tripole_rghost(i,:)
               end do
            endif

            !*** now copy the remaining ghost cell values

            ARRAY(ib_dst:ie_dst,jb_dst:je_dst,dst_block) = &
               tripole_rghost(ib_src:ie_src,:)
         endif

      end do

   endif

!-----------------------------------------------------------------------

end subroutine boundary_2d_real

!***********************************************************************
!BOP
! !IROUTINE: update_ghost_cells
! !INTERFACE:

 subroutine boundary_2d_int(ARRAY, in_bndy, grid_loc, field_type)

! !DESCRIPTION:
!  This routine updates ghost cells for an input array and is a
!  member of a group of routines under the generic interface
!  update\_ghost\_cells.  This routine is the specific interface
!  for 2d horizontal arrays of double precision.
!
! !REVISION HISTORY:
!  same as module

! !INPUT PARAMETERS:

   type (bndy), intent(in) :: &
      in_bndy                 ! boundary update structure for the array

   integer (int_kind), intent(in) :: &
      field_type,               &! id for type of field (scalar, vector, angle)
      grid_loc                   ! id for location on horizontal grid
                                 !  (center, NEcorner, Nface, Eface)

! !INPUT/OUTPUT PARAMETERS:

   integer (int_kind), dimension(:,:,:), intent(inout) :: &
      ARRAY              ! array containing horizontal slab to update

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) ::           &
      i,j,k,m,n,                   &! dummy loop indices
      ib_src,ie_src,jb_src,je_src, &! beg,end indices for bndy cells
      ib_dst,ie_dst,jb_dst,je_dst, &!
      nx_global,                   &! global domain size in x
      src_block,                   &! local block number for source
      dst_block,                   &! local block number for destination
      xoffset, yoffset,            &! address shifts for tripole
      isign                         ! sign factor for tripole grids

   integer (int_kind) :: &
      xavg               ! scalar for enforcing symmetry at U pts

!-----------------------------------------------------------------------
!
!  set nx_global for tripole
!
!-----------------------------------------------------------------------

   if (allocated(tripole_ibuf)) nx_global = size(tripole_ibuf,dim=1)

!-----------------------------------------------------------------------
!
!  do local copies for east-west ghost cell updates
!  also initialize ghost cells to zero
!
!-----------------------------------------------------------------------

   do n=1,in_bndy%nlocal_ew
      src_block = in_bndy%local_ew_src_block(n)
      dst_block = in_bndy%local_ew_dst_block(n)

      ib_src = in_bndy%local_ew_src_add(1,n)
      ie_src = ib_src + nghost - 1
      ib_dst = in_bndy%local_ew_dst_add(1,n)
      ie_dst = ib_dst + nghost - 1

      if (src_block /= 0) then
         ARRAY(ib_dst:ie_dst,:,dst_block) = &
         ARRAY(ib_src:ie_src,:,src_block)
      else
         ARRAY(ib_dst:ie_dst,:,dst_block) = 0
      endif
   end do

!-----------------------------------------------------------------------
!
!  now exchange north-south boundary info
!
!-----------------------------------------------------------------------

   if (allocated(tripole_ibuf)) tripole_ibuf = 0

   do n=1,in_bndy%nlocal_ns
      src_block = in_bndy%local_ns_src_block(n)
      dst_block = in_bndy%local_ns_dst_block(n)

      if (dst_block > 0) then ! straight local copy

         jb_src = in_bndy%local_ns_src_add(2,n)
         je_src = jb_src + nghost - 1
         jb_dst = in_bndy%local_ns_dst_add(2,n)
         je_dst = jb_dst + nghost - 1

         if (src_block /= 0) then
            ARRAY(:,jb_dst:je_dst,dst_block) = &
            ARRAY(:,jb_src:je_src,src_block)
         else
            ARRAY(:,jb_dst:je_dst,dst_block) = 0
         endif

      else  !north boundary tripole grid - copy into global north buffer

         jb_src = in_bndy%local_ns_src_add(2,n)
         je_src = jb_src + nghost ! need nghost+1 rows for tripole

         !*** determine start, end addresses of physical domain
         !*** for both global buffer and local block

         ib_dst = in_bndy%local_ns_src_add(1,n)
         ie_dst = ib_dst + (nx_block-2*nghost) - 1
         if (ie_dst > nx_global) ie_dst = nx_global
         ib_src = nghost + 1
         ie_src = ib_src + ie_dst - ib_dst
         if (src_block /= 0) then
            tripole_ibuf(ib_dst:ie_dst,:) = &
                  ARRAY(ib_src:ie_src,jb_src:je_src,src_block)
         endif

      endif
   end do

!-----------------------------------------------------------------------
!
!  take care of northern boundary in tripole case
!
!-----------------------------------------------------------------------

   if (allocated(tripole_ibuf)) then

      select case (grid_loc)
      case (field_loc_center)   ! cell center location
         xoffset = 1
         yoffset = 1
         !*** first row of ghost cell buffer is actually the last
         !*** row of physical domain (mostly for symmetry enforcement)
         tripole_ighost(:,1) = tripole_ibuf(:,nghost+1)
      case (field_loc_NEcorner)   ! cell corner (velocity) location
         xoffset = 0
         yoffset = 0
         !*** enforce symmetry
         !*** first row of ghost cell buffer is actually the last
         !*** row of physical domain
         do i = 1, nx_global/2
            ib_dst = nx_global - i
            xavg = p5*(abs(tripole_ibuf(i     ,nghost+1)) + &
                       abs(tripole_ibuf(ib_dst,nghost+1)))
            tripole_ighost(i     ,1) = sign(xavg, &
                                          tripole_ibuf(i,nghost+1))
            tripole_ighost(ib_dst,1) = sign(xavg, &
                                          tripole_ibuf(ib_dst,nghost+1))
         end do
         !*** catch nx_global case (should be land anyway...)
         tripole_ighost(nx_global,1) = tripole_ibuf(nx_global,nghost+1)
         tripole_ibuf(:,nghost+1) = tripole_ighost(:,1)
      case (field_loc_Eface)   ! cell center location
         xoffset = 0
         yoffset = 1
         !*** first row of ghost cell buffer is actually the last
         !*** row of physical domain (mostly for symmetry enforcement)
         tripole_ighost(:,1) = tripole_ibuf(:,nghost+1)
      case (field_loc_Nface)   ! cell corner (velocity) location
         xoffset = 1
         yoffset = 0
         !*** enforce symmetry
         !*** first row of ghost cell buffer is actually the last
         !*** row of physical domain
         do i = 1, nx_global/2
            ib_dst = nx_global + 1 - i
            xavg = p5*(abs(tripole_ibuf(i     ,nghost+1)) + &
                       abs(tripole_ibuf(ib_dst,nghost+1)))
            tripole_ighost(i     ,1) = sign(xavg, &
                                          tripole_ibuf(i,nghost+1))
            tripole_ighost(ib_dst,1) = sign(xavg, &
                                          tripole_ibuf(ib_dst,nghost+1))
         end do
         tripole_ibuf(:,nghost+1) = tripole_ighost(:,1)
      case default
         call exit_POP(sigAbort, 'Unknown location in boundary_2d')
      end select

      select case (field_type)
      case (field_type_scalar)
         isign =  1
      case (field_type_vector)
         isign = -1
      case (field_type_angle)
         isign = -1
      case default
         call exit_POP(sigAbort, 'Unknown field type in boundary')
      end select

      !*** copy source (physical) cells into ghost cells
      !*** global source addresses are:
      !*** nx_global + xoffset - i
      !*** ny_global + yoffset - j
      !*** in the actual tripole buffer, the indices are:
      !*** nx_global + xoffset - i = ib_src - i
      !*** ny_global + yoffset - j - (ny_global - nghost) + 1 =
      !***    nghost + yoffset +1 - j = jb_src - j

      ib_src = nx_global + xoffset
      jb_src = nghost + yoffset + 1

      do j=1,nghost
      do i=1,nx_global
         tripole_ighost(i,1+j) = isign* &
                                 tripole_ibuf(ib_src-i, jb_src-j)
      end do
      end do

      !*** copy out of global ghost cell buffer into local
      !*** ghost cells

      do n=1,in_bndy%nlocal_ns
         dst_block = in_bndy%local_ns_dst_block(n)

         if (dst_block < 0) then
            dst_block = -dst_block

            jb_dst = in_bndy%local_ns_dst_add(2,n)
            je_dst = jb_dst + nghost
            ib_src = in_bndy%local_ns_dst_add(1,n)
            !*** ib_src is glob address of 1st point in physical
            !*** domain.  must now adjust to properly copy
            !*** east-west ghost cell info in the north boundary

            if (ib_src == 1) then  ! western boundary
               !*** impose cyclic conditions at western boundary
               !*** then set up remaining indices to copy rest
               !*** of domain from tripole ghost cell buffer
               do i=1,nghost
                  ARRAY(i,jb_dst:je_dst,dst_block) = &
                     tripole_ighost(nx_global-nghost+i,:)
               end do
               ie_src = ib_src + nx_block - nghost - 1
               if (ie_src > nx_global) ie_src = nx_global
               ib_dst = nghost + 1
               ie_dst = ib_dst + (ie_src - ib_src)
            else
               ib_src = ib_src - nghost
               ie_src = ib_src + nx_block - 1
               if (ie_src > nx_global) ie_src = nx_global
               ib_dst = 1
               ie_dst = ib_dst + (ie_src - ib_src)
            endif
            if (ie_src == nx_global) then ! eastern boundary
               !*** impose cyclic conditions in ghost cells
               do i=1,nghost
                  ARRAY(ie_dst+i,jb_dst:je_dst,dst_block) = &
                     tripole_ighost(i,:)
               end do
            endif

            !*** now copy the remaining ghost cell values

            ARRAY(ib_dst:ie_dst,jb_dst:je_dst,dst_block) = &
               tripole_ighost(ib_src:ie_src,:)
         endif

      end do

   endif

!-----------------------------------------------------------------------

end subroutine boundary_2d_int

!***********************************************************************
!BOP
! !IROUTINE: update_ghost_cells
! !INTERFACE:

subroutine boundary_3d_dbl(ARRAY, in_bndy, grid_loc, field_type)

! !DESCRIPTION:
!  This routine updates ghost cells for an input array and is a
!  member of a group of routines under the generic interface
!  update\_ghost\_cells.  This routine is the specific interface
!  for 3d horizontal arrays of double precision.
!
! !REVISION HISTORY:
!  same as module

! !USER:

! !INPUT/OUTPUT PARAMETERS:

   type (bndy), intent(inout) :: &
      in_bndy                 ! boundary update structure for the array

   integer (int_kind), intent(in) :: &
      field_type,               &! id for type of field (scalar, vector, angle)
      grid_loc                   ! id for location on horizontal grid
                                 !  (center, NEcorner, Nface, Eface)

   real (r8), dimension(:,:,:,:), intent(inout) :: &
      ARRAY              ! array containing horizontal slab to update

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) ::           &
      k,m                           ! dummy loop indices

!-----------------------------------------------------------------------

   m = size(ARRAY,3)
   do k = 1, m
      call boundary_2d_dbl(ARRAY(:,:,k,:),in_bndy,grid_loc,field_type)
   end do

!-----------------------------------------------------------------------

end subroutine boundary_3d_dbl

!***********************************************************************
!BOP
! !IROUTINE: update_ghost_cells
! !INTERFACE:

subroutine boundary_3d_real(ARRAY, in_bndy, grid_loc, field_type)

! !DESCRIPTION:
!  This routine updates ghost cells for an input array and is a
!  member of a group of routines under the generic interface
!  update\_ghost\_cells.  This routine is the specific interface
!  for 3d horizontal arrays of single precision.
!
! !REVISION HISTORY:
!  same as module

! !USER:

! !INPUT/OUTPUT PARAMETERS:

   type (bndy), intent(inout) :: &
      in_bndy                 ! boundary update structure for the array

   integer (int_kind), intent(in) :: &
      field_type,               &! id for type of field (scalar, vector, angle)
      grid_loc                   ! id for location on horizontal grid
                                 !  (center, NEcorner, Nface, Eface)

   real (r4), dimension(:,:,:,:), intent(inout) :: &
      ARRAY              ! array containing horizontal slab to update

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) ::           &
      k,m                           ! dummy loop indices

!-----------------------------------------------------------------------

   m = size(ARRAY,3)
   do k = 1, m
      call boundary_2d_real(ARRAY(:,:,k,:),in_bndy,grid_loc,field_type)
   end do

!-----------------------------------------------------------------------

end subroutine boundary_3d_real

!***********************************************************************
!BOP
! !IROUTINE: update_ghost_cells
! !INTERFACE:

subroutine boundary_3d_int(ARRAY, in_bndy, grid_loc, field_type)

! !DESCRIPTION:
!  This routine updates ghost cells for an input array and is a
!  member of a group of routines under the generic interface
!  update\_ghost\_cells.  This routine is the specific interface
!  for 3d horizontal arrays of integer.
!
! !REVISION HISTORY:
!  same as module

! !USER:

! !INPUT/OUTPUT PARAMETERS:

   type (bndy), intent(inout) :: &
      in_bndy                 ! boundary update structure for the array

   integer (int_kind), intent(in) :: &
      field_type,               &! id for type of field (scalar, vector, angle)
      grid_loc                   ! id for location on horizontal grid
                                 !  (center, NEcorner, Nface, Eface)

   integer (int_kind), dimension(:,:,:,:), intent(inout) :: &
      ARRAY              ! array containing horizontal slab to update

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) ::           &
      k,m                           ! dummy loop indices

!-----------------------------------------------------------------------

   m = size(ARRAY,3)
   do k = 1, m
      call boundary_2d_int(ARRAY(:,:,k,:),in_bndy,grid_loc,field_type)
   end do

!-----------------------------------------------------------------------

end subroutine boundary_3d_int

!***********************************************************************
!BOP
! !IROUTINE: update_ghost_cells
! !INTERFACE:

subroutine boundary_4d_dbl(ARRAY, in_bndy, grid_loc, field_type)

! !DESCRIPTION:
!  This routine updates ghost cells for an input array and is a
!  member of a group of routines under the generic interface
!  update\_ghost\_cells.  This routine is the specific interface
!  for 3d horizontal arrays of double precision.
!
! !REVISION HISTORY:
!  same as module

! !USER:

! !INPUT/OUTPUT PARAMETERS:

   type (bndy), intent(inout) :: &
      in_bndy                 ! boundary update structure for the array

   integer (int_kind), intent(in) :: &
      field_type,               &! id for type of field (scalar, vector, angle)
      grid_loc                   ! id for location on horizontal grid
                                 !  (center, NEcorner, Nface, Eface)

   real (r8), dimension(:,:,:,:,:), intent(inout) :: &
      ARRAY              ! array containing horizontal slab to update

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) ::           &
      k,l,m,n                      ! dummy loop indices

!-----------------------------------------------------------------------

   l = size(ARRAY,dim=3)
   n = size(ARRAY,dim=4)
   do k=1,l
   do m=1,n
      call boundary_2d_dbl(ARRAY(:,:,k,m,:),in_bndy,grid_loc,field_type)
   end do
   end do

!-----------------------------------------------------------------------

end subroutine boundary_4d_dbl

!***********************************************************************
!BOP
! !IROUTINE: update_ghost_cells
! !INTERFACE:

subroutine boundary_4d_real(ARRAY, in_bndy, grid_loc, field_type)

! !DESCRIPTION:
!  This routine updates ghost cells for an input array and is a
!  member of a group of routines under the generic interface
!  update\_ghost\_cells.  This routine is the specific interface
!  for 3d horizontal arrays of single precision.
!
! !REVISION HISTORY:
!  same as module

! !USER:

! !INPUT/OUTPUT PARAMETERS:

   type (bndy), intent(inout) :: &
      in_bndy                 ! boundary update structure for the array

   integer (int_kind), intent(in) :: &
      field_type,               &! id for type of field (scalar, vector, angle)
      grid_loc                   ! id for location on horizontal grid
                                 !  (center, NEcorner, Nface, Eface)

   real (r4), dimension(:,:,:,:,:), intent(inout) :: &
      ARRAY              ! array containing horizontal slab to update

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) ::           &
      k,l,m,n                      ! dummy loop indices

!-----------------------------------------------------------------------

   l = size(ARRAY,dim=3)
   n = size(ARRAY,dim=4)
   do k=1,l
   do m=1,n
     call boundary_2d_real(ARRAY(:,:,k,m,:),in_bndy,grid_loc,field_type)
   end do
   end do

!-----------------------------------------------------------------------

end subroutine boundary_4d_real

!***********************************************************************
!BOP
! !IROUTINE: update_ghost_cells
! !INTERFACE:

subroutine boundary_4d_int(ARRAY, in_bndy, grid_loc, field_type)

! !DESCRIPTION:
!  This routine updates ghost cells for an input array and is a
!  member of a group of routines under the generic interface
!  update\_ghost\_cells.  This routine is the specific interface
!  for 3d horizontal arrays of double precision.
!
! !REVISION HISTORY:
!  same as module

! !USER:

! !INPUT/OUTPUT PARAMETERS:

   type (bndy), intent(inout) :: &
      in_bndy                 ! boundary update structure for the array

   integer (int_kind), intent(in) :: &
      field_type,               &! id for type of field (scalar, vector, angle)
      grid_loc                   ! id for location on horizontal grid
                                 !  (center, NEcorner, Nface, Eface)

   integer (int_kind), dimension(:,:,:,:,:), intent(inout) :: &
      ARRAY              ! array containing horizontal slab to update

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) ::           &
      k,l,m,n                      ! dummy loop indices

!-----------------------------------------------------------------------

   l = size(ARRAY,dim=3)
   n = size(ARRAY,dim=4)

   do k=1,l
   do m=1,n
      call boundary_2d_int(ARRAY(:,:,k,m,:),in_bndy,grid_loc,field_type)
   end do
   end do

!-----------------------------------------------------------------------

end subroutine boundary_4d_int

!EOC
!***********************************************************************

end module boundary

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
