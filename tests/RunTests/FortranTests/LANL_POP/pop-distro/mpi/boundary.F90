!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
!BOP
! !MODULE: boundary

 module boundary

! !DESCRIPTION:
!  This module contains data types and routines for updating ghost cell
!  boundaries using MPI calls
!
! !REVISION HISTORY:
!  CVS:$Id: boundary.F90,v 1.13 2004/01/07 19:56:32 pwjones Exp $
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
       nmsg_ew_snd        ,&! number of messages to send for e-w update
       nmsg_ns_snd        ,&! number of messages to send for n-s update
       nmsg_ew_rcv        ,&! number of messages to recv for e-w update
       nmsg_ns_rcv        ,&! number of messages to recv for n-s update
       maxblocks_ew_snd   ,&! max num blocks involved in east-west sends
       maxblocks_ew_rcv   ,&! max num blocks involved in east-west recvs
       maxblocks_ns_snd   ,&! max num blocks involved in north-south sends
       maxblocks_ns_rcv   ,&! max num blocks involved in north-south recvs
       nlocal_ew          ,&! num local copies for east-west bndy update
       nlocal_ns            ! num local copies for east-west bndy update

     integer (int_kind), dimension(:), pointer :: &
       nblocks_ew_snd     ,&! num blocks in each east-west send msg
       nblocks_ns_snd     ,&! num blocks in each north-south send msg
       nblocks_ew_rcv     ,&! num blocks in each east-west recv msg
       nblocks_ns_rcv     ,&! num blocks in each north-south recv msg
       ew_snd_proc        ,&! dest   proc for east-west send message
       ew_rcv_proc        ,&! source proc for east-west recv message
       ns_snd_proc        ,&! dest   proc for north-south send message
       ns_rcv_proc        ,&! source proc for north-south recv message
       local_ew_src_block ,&! source block for each local east-west copy
       local_ew_dst_block ,&! dest   block for each local east-west copy
       local_ns_src_block ,&! source block for each local north-south copy
       local_ns_dst_block   ! dest   block for each local north-south copy

     integer (int_kind), dimension(:,:), pointer :: &
       local_ew_src_add   ,&! starting source address for local e-w copies
       local_ew_dst_add   ,&! starting dest   address for local e-w copies
       local_ns_src_add   ,&! starting source address for local n-s copies
       local_ns_dst_add     ! starting dest   address for local n-s copies

     integer (int_kind), dimension(:,:), pointer :: &
       ew_src_block       ,&! source block for sending   e-w bndy msg
       ew_dst_block       ,&! dest   block for receiving e-w bndy msg
       ns_src_block       ,&! source block for sending   n-s bndy msg
       ns_dst_block         ! dest   block for sending   n-s bndy msg

     integer (int_kind), dimension(:,:,:), pointer :: &
       ew_src_add         ,&! starting source address for e-w msgs
       ew_dst_add         ,&! starting dest   address for e-w msgs
       ns_src_add         ,&! starting source address for n-s msgs
       ns_dst_add           ! starting dest   address for n-s msgs
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
      imsg_ew_snd, imsg_ew_rcv,    &! counters for ew send/recv
      imsg_ns_snd, imsg_ns_rcv,    &! counters for ns send/recv
      nprocs,                      &! num of processors involved
      nblocks,                     &! total number of blocks
      bid, pid,                    &! block and processor locators
      iblk, imsg,                  &!
      iloc_ew, iloc_ns,            &!
      src_proc, dst_proc            ! src,dst processor for message

   logical (log_kind) :: &
      lalloc_tripole      ! flag for allocating tripole buffers

   integer (int_kind), dimension(:), allocatable :: &
      ew_snd_count,    &! array for counting blocks in each message
      ew_rcv_count,    &! array for counting blocks in each message
      ns_snd_count,    &! array for counting blocks in each message
      ns_rcv_count,    &! array for counting blocks in each message
      msg_ew_snd  ,    &! msg counter for each active processor
      msg_ew_rcv  ,    &! msg counter for each active processor
      msg_ns_snd  ,    &! msg counter for each active processor
      msg_ns_rcv        ! msg counter for each active processor

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

   allocate (ew_snd_count(nprocs), ew_rcv_count(nprocs), &
             ns_snd_count(nprocs), ns_rcv_count(nprocs))

   ew_snd_count = 0
   ew_rcv_count = 0
   ns_snd_count = 0
   ns_rcv_count = 0

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
      !*** if any boundary is closed boundary, create a local
      !*** copy pseudo-message to fill ghost cells
      !***

      if (iblock_east == 0) &
         call increment_message_counter(ew_snd_count, ew_rcv_count, &
                                        0, src_proc)
      if (iblock_west == 0) &
         call increment_message_counter(ew_snd_count, ew_rcv_count, &
                                        0, src_proc)
      if (jblock_north == 0) &
         call increment_message_counter(ns_snd_count, ns_rcv_count, &
                                        0, src_proc)
      if (jblock_south == 0) &
         call increment_message_counter(ns_snd_count, ns_rcv_count, &
                                        0, src_proc)

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

            call increment_message_counter(ew_snd_count, ew_rcv_count, &
                                           src_proc, dst_proc)
         endif

         !***
         !*** if this block is an western neighbor
         !*** increment message counter
         !***

         if (iblock_dst == iblock_west .and. &
             jblock_dst == jblock_west) then

            call increment_message_counter(ew_snd_count, ew_rcv_count, &
                                           src_proc, dst_proc)
         endif

         !***
         !*** if this block is an northern neighbor
         !*** find out whether a message is required
         !*** for tripole, must communicate with all
         !*** north row blocks (triggered by iblock_dst <0)
         !***

         if ((iblock_dst == iblock_north .or. iblock_north < 0) .and. &
              jblock_dst == jblock_north) then

            call increment_message_counter(ns_snd_count, ns_rcv_count, &
                                           src_proc, dst_proc)
         endif

         !***
         !*** if this block is an southern neighbor
         !*** find out whether a message is required
         !***

         if (iblock_dst == iblock_south .and. &
             jblock_dst == jblock_south) then

            call increment_message_counter(ns_snd_count, ns_rcv_count, &
                                           src_proc, dst_proc)
         endif

      end do  ! search for dest blocks
   end do block_loop1

   !*** if messages are received from the same processor
   !*** the message is actually a local copy - count them
   !*** and reset to zero

   newbndy%nlocal_ew = ew_rcv_count(my_task+1)
   newbndy%nlocal_ns = ns_rcv_count(my_task+1)
   ew_snd_count(my_task+1) = 0
   ew_rcv_count(my_task+1) = 0
   ns_snd_count(my_task+1) = 0
   ns_rcv_count(my_task+1) = 0

   !*** now count the number of actual messages to be
   !*** sent and received

   newbndy%nmsg_ew_snd = count(ew_snd_count /= 0)
   newbndy%nmsg_ns_snd = count(ns_snd_count /= 0)
   newbndy%nmsg_ew_rcv = count(ew_rcv_count /= 0)
   newbndy%nmsg_ns_rcv = count(ns_rcv_count /= 0)

   !*** find the maximum number of blocks sent in any one
   !*** message to use as an array size parameter

   newbndy%maxblocks_ew_snd = maxval(ew_snd_count)
   newbndy%maxblocks_ew_rcv = maxval(ew_rcv_count)
   newbndy%maxblocks_ns_snd = maxval(ns_snd_count)
   newbndy%maxblocks_ns_rcv = maxval(ns_rcv_count)

   !***
   !*** create buffers for tracking which message
   !*** is sent/received from which processor
   !***

   allocate(msg_ew_snd(nprocs), msg_ew_rcv(nprocs), &
            msg_ns_snd(nprocs), msg_ns_rcv(nprocs))

   msg_ew_snd = 0
   msg_ew_rcv = 0
   msg_ns_snd = 0
   msg_ns_rcv = 0

   !***
   !*** assign a location in buffer for each message to a
   !*** different processor. scramble the processor order
   !*** using current task id as offset to prevent all
   !*** processors sending to the same processor at the
   !*** same time
   !***

   imsg_ew_snd = 0
   imsg_ew_rcv = 0
   imsg_ns_snd = 0
   imsg_ns_rcv = 0

   do n=1,nprocs
      dst_proc = modulo(my_task+n,nprocs) + 1
      if (ew_snd_count(dst_proc) /= 0) then
         imsg_ew_snd = imsg_ew_snd + 1
         msg_ew_snd(dst_proc) = imsg_ew_snd
      endif
      if (ew_rcv_count(dst_proc) /= 0) then
         imsg_ew_rcv = imsg_ew_rcv + 1
         msg_ew_rcv(dst_proc) = imsg_ew_rcv
      endif
      if (ns_snd_count(dst_proc) /= 0) then
         imsg_ns_snd = imsg_ns_snd + 1
         msg_ns_snd(dst_proc) = imsg_ns_snd
      endif
      if (ns_rcv_count(dst_proc) /= 0) then
         imsg_ns_rcv = imsg_ns_rcv + 1
         msg_ns_rcv(dst_proc) = imsg_ns_rcv
      endif
   end do

   deallocate(ew_snd_count, ew_rcv_count, ns_snd_count, ns_rcv_count)

!-----------------------------------------------------------------------
!
!  allocate buffers and arrays necessary for boundary comms
!
!-----------------------------------------------------------------------

   allocate (newbndy%nblocks_ew_snd(newbndy%nmsg_ew_snd), &
             newbndy%nblocks_ns_snd(newbndy%nmsg_ns_snd), &
             newbndy%nblocks_ew_rcv(newbndy%nmsg_ew_rcv), &
             newbndy%nblocks_ns_rcv(newbndy%nmsg_ns_rcv))

   allocate (newbndy%local_ew_src_block(newbndy%nlocal_ew), &
             newbndy%local_ew_dst_block(newbndy%nlocal_ew), &
             newbndy%local_ns_src_block(newbndy%nlocal_ns), &
             newbndy%local_ns_dst_block(newbndy%nlocal_ns), &
             newbndy%local_ew_src_add(2,newbndy%nlocal_ew), &
             newbndy%local_ew_dst_add(2,newbndy%nlocal_ew), &
             newbndy%local_ns_src_add(2,newbndy%nlocal_ns), &
             newbndy%local_ns_dst_add(2,newbndy%nlocal_ns))

   allocate ( &
     newbndy%ew_snd_proc (newbndy%nmsg_ew_snd), &
     newbndy%ew_rcv_proc (newbndy%nmsg_ew_rcv), &
     newbndy%ns_snd_proc (newbndy%nmsg_ns_snd), &
     newbndy%ns_rcv_proc (newbndy%nmsg_ns_rcv), &
     newbndy%ew_src_block(newbndy%maxblocks_ew_snd,newbndy%nmsg_ew_snd), &
     newbndy%ew_dst_block(newbndy%maxblocks_ew_rcv,newbndy%nmsg_ew_rcv), &
     newbndy%ns_src_block(newbndy%maxblocks_ns_snd,newbndy%nmsg_ns_snd), &
     newbndy%ns_dst_block(newbndy%maxblocks_ns_rcv,newbndy%nmsg_ns_rcv), &
     newbndy%ew_src_add(2,newbndy%maxblocks_ew_snd,newbndy%nmsg_ew_snd), &
     newbndy%ew_dst_add(2,newbndy%maxblocks_ew_rcv,newbndy%nmsg_ew_rcv), &
     newbndy%ns_src_add(2,newbndy%maxblocks_ns_snd,newbndy%nmsg_ns_snd), &
     newbndy%ns_dst_add(3,newbndy%maxblocks_ns_rcv,newbndy%nmsg_ns_rcv))

   newbndy%nblocks_ew_snd = 0
   newbndy%nblocks_ns_snd = 0
   newbndy%nblocks_ew_rcv = 0
   newbndy%nblocks_ns_rcv = 0

   newbndy%ew_snd_proc = 0
   newbndy%ew_rcv_proc = 0
   newbndy%ns_snd_proc = 0
   newbndy%ns_rcv_proc = 0

   newbndy%local_ew_src_block = 0
   newbndy%local_ew_dst_block = 0
   newbndy%local_ns_src_block = 0
   newbndy%local_ns_dst_block = 0
   newbndy%local_ew_src_add = 0
   newbndy%local_ew_dst_add = 0
   newbndy%local_ns_src_add = 0
   newbndy%local_ns_dst_add = 0

   newbndy%ew_src_block = 0
   newbndy%ew_dst_block = 0
   newbndy%ns_src_block = 0
   newbndy%ns_dst_block = 0
   newbndy%ew_src_add = 0
   newbndy%ew_dst_add = 0
   newbndy%ns_src_add = 0
   newbndy%ns_dst_add = 0

!-----------------------------------------------------------------------
!
!  now set up indices into buffers and address arrays
!
!-----------------------------------------------------------------------

   allocate (ew_snd_count(newbndy%nmsg_ew_snd), &
             ew_rcv_count(newbndy%nmsg_ew_rcv), &
             ns_snd_count(newbndy%nmsg_ns_snd), &
             ns_rcv_count(newbndy%nmsg_ns_rcv))

   ew_snd_count = 0
   ew_rcv_count = 0
   ns_snd_count = 0
   ns_rcv_count = 0

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
      !*** if blocks are at closed boundary, create local copy
      !*** pseudo-message to fill ghost cells
      !***

      if (src_block_loc /= 0 .and. src_proc == my_task+1) then
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

               if (src_proc == my_task+1 .and. &
                   src_proc == dst_proc) then
                  !*** local copy from one block to another
                  iloc_ew = iloc_ew + 1
                  newbndy%local_ew_src_block(iloc_ew) = src_block_loc
                  newbndy%local_ew_src_add(1,iloc_ew) = src_block%ie - &
                                                        nghost + 1
                  newbndy%local_ew_src_add(2,iloc_ew) = 1
                  newbndy%local_ew_dst_block(iloc_ew) = dst_block_loc
                  newbndy%local_ew_dst_add(1,iloc_ew) = 1
                  newbndy%local_ew_dst_add(2,iloc_ew) = 1
               else if (src_proc == 0 .and. dst_proc == my_task+1) then
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
               else if (src_proc == my_task+1 .and. &
                        dst_proc /= my_task+1) then
                  !*** an actual message must be sent
                  imsg = msg_ew_snd(dst_proc)
                  ew_snd_count(imsg) = ew_snd_count(imsg) + 1
                  iblk = ew_snd_count(imsg)
                  newbndy%ew_snd_proc (     imsg) = dst_proc
                  newbndy%ew_src_block(iblk,imsg) = src_block_loc
                  newbndy%ew_src_add(1,iblk,imsg) = src_block%ie - &
                                                    nghost + 1
                  newbndy%ew_src_add(2,iblk,imsg) = 1
                  newbndy%nblocks_ew_snd(imsg) = &
                  newbndy%nblocks_ew_snd(imsg) + 1
               else if (dst_proc == my_task+1 .and. &
                        src_proc /= my_task+1) then
                  !*** must receive a message
                  imsg = msg_ew_rcv(src_proc)
                  ew_rcv_count(imsg) = ew_rcv_count(imsg) + 1
                  iblk = ew_rcv_count(imsg)
                  newbndy%ew_rcv_proc (     imsg) = src_proc
                  newbndy%ew_dst_block(iblk,imsg) = dst_block_loc
                  newbndy%ew_dst_add(:,iblk,imsg) = 1
                  newbndy%nblocks_ew_rcv(imsg) = &
                  newbndy%nblocks_ew_rcv(imsg) + 1
               endif

            endif ! east neighbor

            !***
            !*** if this block is a western neighbor
            !*** determine send/receive addresses
            !***

            if (iblock_dst == iblock_west .and. &
                jblock_dst == jblock_west) then

               if (src_proc == my_task+1 .and. &
                   src_proc == dst_proc) then
                  !*** perform a local copy
                  iloc_ew = iloc_ew + 1
                  newbndy%local_ew_src_block(iloc_ew) = src_block_loc
                  newbndy%local_ew_src_add(1,iloc_ew) = nghost + 1
                  newbndy%local_ew_src_add(2,iloc_ew) = 1
                  newbndy%local_ew_dst_block(iloc_ew) = dst_block_loc
                  newbndy%local_ew_dst_add(1,iloc_ew) = dst_block%ie + 1
                  newbndy%local_ew_dst_add(2,iloc_ew) = 1
               else if (src_proc == 0 .and. dst_proc == my_task+1) then
                  !*** neighbor is a land block so zero ghost cells
                  iloc_ew = iloc_ew + 1
                  newbndy%local_ew_src_block(iloc_ew) = 0
                  newbndy%local_ew_src_add(1,iloc_ew) = 0
                  newbndy%local_ew_src_add(2,iloc_ew) = 0
                  newbndy%local_ew_dst_block(iloc_ew) = dst_block_loc
                  newbndy%local_ew_dst_add(1,iloc_ew) = dst_block%ie + 1
                  newbndy%local_ew_dst_add(2,iloc_ew) = 1
               else if (src_proc == my_task+1 .and. &
                        dst_proc /= my_task+1) then
                  !*** message must be sent
                  imsg = msg_ew_snd(dst_proc)
                  ew_snd_count(imsg) = ew_snd_count(imsg) + 1
                  iblk = ew_snd_count(imsg)
                  newbndy%ew_snd_proc (     imsg) = dst_proc
                  newbndy%ew_src_block(iblk,imsg) = src_block_loc
                  newbndy%ew_src_add(1,iblk,imsg) = nghost + 1
                  newbndy%ew_src_add(2,iblk,imsg) = 1
                  newbndy%nblocks_ew_snd(imsg) = &
                  newbndy%nblocks_ew_snd(imsg) + 1
               else if (dst_proc == my_task+1 .and. &
                        src_proc /= my_task+1) then
                  !*** message must be received
                  imsg = msg_ew_rcv(src_proc)
                  ew_rcv_count(imsg) = ew_rcv_count(imsg) + 1
                  iblk = ew_rcv_count(imsg)
                  newbndy%ew_rcv_proc (     imsg) = src_proc
                  newbndy%ew_dst_block(iblk,imsg) = dst_block_loc
                  newbndy%ew_dst_add(1,iblk,imsg) = dst_block%ie + 1
                  newbndy%ew_dst_add(2,iblk,imsg) = 1
                  newbndy%nblocks_ew_rcv(imsg) = &
                  newbndy%nblocks_ew_rcv(imsg) + 1
               endif

            endif ! west neighbor

            !***
            !*** if this block is a northern neighbor
            !***  compute send/recv addresses
            !*** for tripole, must communicate with all
            !*** north row blocks (triggered by iblock_dst <0)
            !***

            if ((iblock_dst == iblock_north .or. iblock_north < 0) .and. &
                 jblock_dst == jblock_north) then

               if (src_proc == my_task+1 .and. &
                   src_proc == dst_proc) then
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
               else if (src_proc == 0 .and. dst_proc == my_task+1) then
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
               else if (src_proc == my_task+1 .and. &
                        dst_proc /= my_task+1) then
                  !*** message must be sent
                  imsg = msg_ns_snd(dst_proc)
                  ns_snd_count(imsg) = ns_snd_count(imsg) + 1
                  iblk = ns_snd_count(imsg)
                  newbndy%ns_snd_proc (     imsg) = dst_proc
                  newbndy%ns_src_block(iblk,imsg) = src_block_loc
                  newbndy%ns_src_add(1,iblk,imsg) = 1
                  newbndy%ns_src_add(2,iblk,imsg) = src_block%je - &
                                                    nghost + 1
                  newbndy%nblocks_ns_snd(imsg) = &
                  newbndy%nblocks_ns_snd(imsg) + 1
                  if (iblock_north < 0) then !*** tripole boundary
                     !*** need extra ghost cell for U points
                     newbndy%ns_src_add(2,iblk,imsg) = src_block%je - nghost
                  endif
               else if (dst_proc == my_task+1 .and. &
                        src_proc /= my_task+1) then
                  !*** message must be received
                  imsg = msg_ns_rcv(src_proc)
                  ns_rcv_count(imsg) = ns_rcv_count(imsg) + 1
                  iblk = ns_rcv_count(imsg)
                  newbndy%ns_rcv_proc (     imsg) = src_proc
                  newbndy%ns_dst_block(iblk,imsg) = dst_block_loc
                  newbndy%ns_dst_add(1,iblk,imsg) = 1
                  newbndy%ns_dst_add(2,iblk,imsg) = 1
                  newbndy%nblocks_ns_rcv(imsg) = &
                  newbndy%nblocks_ns_rcv(imsg) + 1
                  if (iblock_north < 0) then !*** tripole
                     newbndy%ns_dst_block(iblk,imsg) = -dst_block_loc
                     !*** upon receiving message, store in global 
                     !*** tripole buffer for src, then copy out of
                     !*** ghost cell buffer once global buffer filled
                     !*** i address for storing in global buffer
                     newbndy%ns_dst_add(1,iblk,imsg) = &
                                            src_block%i_glob(nghost+1)
                     !*** addresses for copying out of ghost buffer
                     newbndy%ns_dst_add(2,iblk,imsg) = dst_block%je
                     newbndy%ns_dst_add(3,iblk,imsg) = &
                                       dst_block%i_glob(nghost+1)
                  endif
               endif

            endif ! north neighbor

            !***
            !*** if this block is a southern neighbor
            !*** determine send/receive addresses
            !***

            if (iblock_dst == iblock_south .and. &
                jblock_dst == jblock_south) then

               if (src_proc == my_task+1 .and. &
                   src_proc == dst_proc) then
                  !*** local copy
                  iloc_ns = iloc_ns + 1
                  newbndy%local_ns_src_block(iloc_ns) = src_block_loc
                  newbndy%local_ns_src_add(1,iloc_ns) = 1
                  newbndy%local_ns_src_add(2,iloc_ns) = nghost + 1
                  newbndy%local_ns_dst_block(iloc_ns) = dst_block_loc
                  newbndy%local_ns_dst_add(1,iloc_ns) = 1
                  newbndy%local_ns_dst_add(2,iloc_ns) = dst_block%je + 1
               else if (src_proc == 0 .and. dst_proc == my_task+1) then
                  !*** neighbor is a land block so zero ghost cells
                  iloc_ns = iloc_ns + 1
                  newbndy%local_ns_src_block(iloc_ns) = 0
                  newbndy%local_ns_src_add(1,iloc_ns) = 0
                  newbndy%local_ns_src_add(2,iloc_ns) = 0
                  newbndy%local_ns_dst_block(iloc_ns) = dst_block_loc
                  newbndy%local_ns_dst_add(1,iloc_ns) = 1
                  newbndy%local_ns_dst_add(2,iloc_ns) = dst_block%je + 1
               else if (src_proc == my_task+1 .and. &
                        dst_proc /= my_task+1) then
                  !*** message must be sent
                  imsg = msg_ns_snd(dst_proc)
                  ns_snd_count(imsg) = ns_snd_count(imsg) + 1
                  iblk = ns_snd_count(imsg)
                  newbndy%ns_snd_proc (     imsg) = dst_proc
                  newbndy%ns_src_block(iblk,imsg) = src_block_loc
                  newbndy%ns_src_add(1,iblk,imsg) = 1
                  newbndy%ns_src_add(2,iblk,imsg) = nghost+1
                  newbndy%nblocks_ns_snd(imsg) = &
                  newbndy%nblocks_ns_snd(imsg) + 1
               else if (dst_proc == my_task+1 .and. &
                        src_proc /= my_task+1) then
                  !*** message must be received
                  imsg = msg_ns_rcv(src_proc)
                  ns_rcv_count(imsg) = ns_rcv_count(imsg) + 1
                  iblk = ns_rcv_count(imsg)
                  newbndy%ns_rcv_proc (     imsg) = src_proc
                  newbndy%ns_dst_block(iblk,imsg) = dst_block_loc
                  newbndy%ns_dst_add(1,iblk,imsg) = 1
                  newbndy%ns_dst_add(2,iblk,imsg) = dst_block%je + 1
                  newbndy%nblocks_ns_rcv(imsg) = &
                  newbndy%nblocks_ns_rcv(imsg) + 1
               endif
            endif ! south neighbor

         endif  ! not a land block

      end do
   end do block_loop2

!-----------------------------------------------------------------------

   deallocate(ew_snd_count, ew_rcv_count, ns_snd_count, ns_rcv_count)
   deallocate(msg_ew_snd, msg_ew_rcv, msg_ns_snd, msg_ns_rcv)

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
   in_bndy%nmsg_ew_snd       = 0
   in_bndy%nmsg_ns_snd       = 0
   in_bndy%nmsg_ew_rcv       = 0
   in_bndy%nmsg_ns_rcv       = 0
   in_bndy%maxblocks_ew_snd  = 0
   in_bndy%maxblocks_ew_rcv  = 0
   in_bndy%maxblocks_ns_snd  = 0
   in_bndy%maxblocks_ns_rcv  = 0
   in_bndy%nlocal_ew         = 0
   in_bndy%nlocal_ns         = 0

!-----------------------------------------------------------------------
!
!  deallocate all pointers
!
!-----------------------------------------------------------------------

   deallocate(in_bndy%nblocks_ew_snd,     &
              in_bndy%nblocks_ns_snd,     &
              in_bndy%nblocks_ew_rcv,     &
              in_bndy%nblocks_ns_rcv,     &
              in_bndy%ew_snd_proc,        &
              in_bndy%ew_rcv_proc,        &
              in_bndy%ns_snd_proc,        &
              in_bndy%ns_rcv_proc,        &
              in_bndy%local_ew_src_block, &
              in_bndy%local_ew_dst_block, &
              in_bndy%local_ns_src_block, &
              in_bndy%local_ns_dst_block, &
              in_bndy%local_ew_src_add,   &
              in_bndy%local_ew_dst_add,   &
              in_bndy%local_ns_src_add,   &
              in_bndy%local_ns_dst_add,   &
              in_bndy%ew_src_block,       &
              in_bndy%ew_dst_block,       &
              in_bndy%ns_src_block,       &
              in_bndy%ns_dst_block,       &
              in_bndy%ew_src_add,         &
              in_bndy%ew_dst_add,         &
              in_bndy%ns_src_add,         &
              in_bndy%ns_dst_add )

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

! !USER:

   include 'mpif.h'   ! MPI Fortran include file

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
      bufsize,                     &! buffer size for send/recv buffers
      xoffset, yoffset,            &! address shifts for tripole
      isign,                       &! sign factor for tripole grids
      ierr                          ! MPI error flag

   integer (int_kind), dimension(:), allocatable :: &
      snd_request,              &! MPI request ids
      rcv_request                ! MPI request ids

   integer (int_kind), dimension(:,:), allocatable :: &
      snd_status,               &! MPI status flags
      rcv_status                 ! MPI status flags

   real (r8), dimension(:,:,:,:), allocatable :: &
      buf_ew_snd,       &! message buffer for east-west sends
      buf_ew_rcv,       &! message buffer for east-west recvs
      buf_ns_snd,       &! message buffer for north-south sends
      buf_ns_rcv         ! message buffer for north-south recvs

   real (r8) :: &
      xavg               ! scalar for enforcing symmetry at U pts

   !logical (log_kind), save :: first_call = .true.
   !integer (int_kind), save :: bndy_2d_local, bndy_2d_recv, &
   !                            bndy_2d_send, bndy_2d_wait, bndy_2d_final

!-----------------------------------------------------------------------
!
!  allocate buffers for east-west sends and receives
!
!-----------------------------------------------------------------------

   !if (first_call) then
   !  first_call = .false.
   !  call get_timer(bndy_2d_local, 'BNDY_2D_LOCAL')
   !  call get_timer(bndy_2d_recv,  'BNDY_2D_RECV')
   !  call get_timer(bndy_2d_send,  'BNDY_2D_SEND')
   !  call get_timer(bndy_2d_wait,  'BNDY_2D_WAIT')
   !  call get_timer(bndy_2d_final, 'BNDY_2D_FINAL')
   !endif

   allocate(buf_ew_snd(nghost, ny_block, &
                       in_bndy%maxblocks_ew_snd, in_bndy%nmsg_ew_snd),&
            buf_ew_rcv(nghost, ny_block, &
                       in_bndy%maxblocks_ew_rcv, in_bndy%nmsg_ew_rcv))

   allocate(snd_request(in_bndy%nmsg_ew_snd), &
            rcv_request(in_bndy%nmsg_ew_rcv), &
            snd_status(MPI_STATUS_SIZE,in_bndy%nmsg_ew_snd), &
            rcv_status(MPI_STATUS_SIZE,in_bndy%nmsg_ew_rcv))

   if (allocated(tripole_dbuf)) nx_global = size(tripole_dbuf,dim=1)

!-----------------------------------------------------------------------
!
!  post receives
!
!-----------------------------------------------------------------------

   !call timer_start(bndy_2d_recv)
   do n=1,in_bndy%nmsg_ew_rcv

      bufsize = ny_block*nghost*in_bndy%nblocks_ew_rcv(n)

      call MPI_IRECV(buf_ew_rcv(1,1,1,n), bufsize, mpi_dbl,   &
                     in_bndy%ew_rcv_proc(n)-1,                &
                     mpitag_bndy_2d + in_bndy%ew_rcv_proc(n), &
                     in_bndy%communicator, rcv_request(n), ierr)
   end do
   !call timer_stop(bndy_2d_recv)

!-----------------------------------------------------------------------
!
!  fill send buffer and post sends
!
!-----------------------------------------------------------------------

   !call timer_start(bndy_2d_send)
   do n=1,in_bndy%nmsg_ew_snd

      bufsize = ny_block*nghost*in_bndy%nblocks_ew_snd(n)

      do i=1,in_bndy%nblocks_ew_snd(n)
         ib_src    = in_bndy%ew_src_add(1,i,n)
         ie_src    = ib_src + nghost - 1
         src_block = in_bndy%ew_src_block(i,n)
         buf_ew_snd(:,:,i,n) = ARRAY(ib_src:ie_src,:,src_block)
      end do

      call MPI_ISEND(buf_ew_snd(1,1,1,n), bufsize, mpi_dbl, &
                     in_bndy%ew_snd_proc(n)-1, &
                     mpitag_bndy_2d + my_task + 1, &
                     in_bndy%communicator, snd_request(n), ierr)
   end do
   !call timer_stop(bndy_2d_send)

!-----------------------------------------------------------------------
!
!  do local copies while waiting for messages to complete
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
   !call timer_stop(bndy_2d_local)

!-----------------------------------------------------------------------
!
!  wait for receives to finish and then unpack the recv buffer into
!  ghost cells
!
!-----------------------------------------------------------------------

   !call timer_start(bndy_2d_wait)
   call MPI_WAITALL(in_bndy%nmsg_ew_rcv, rcv_request, rcv_status, ierr)
   !call timer_stop(bndy_2d_wait)

   !call timer_start(bndy_2d_final)
   do n=1,in_bndy%nmsg_ew_rcv
   do k=1,in_bndy%nblocks_ew_rcv(n)
      dst_block = in_bndy%ew_dst_block(k,n)

      ib_dst = in_bndy%ew_dst_add(1,k,n)
      ie_dst = ib_dst + nghost - 1

      ARRAY(ib_dst:ie_dst,:,dst_block) = buf_ew_rcv(:,:,k,n)
   end do
   end do
   !call timer_stop(bndy_2d_final)

!-----------------------------------------------------------------------
!
!  wait for sends to complete and deallocate arrays
!
!-----------------------------------------------------------------------

   !call timer_start(bndy_2d_wait)
   call MPI_WAITALL(in_bndy%nmsg_ew_snd, snd_request, snd_status, ierr)
   !call timer_stop(bndy_2d_wait)

   deallocate(buf_ew_snd, buf_ew_rcv)
   deallocate(snd_request, rcv_request, snd_status, rcv_status)

!-----------------------------------------------------------------------
!
!  now exchange north-south boundary info
!
!-----------------------------------------------------------------------

   allocate(buf_ns_snd(nx_block, nghost+1, &
                       in_bndy%maxblocks_ns_snd, in_bndy%nmsg_ns_snd),&
            buf_ns_rcv(nx_block, nghost+1, &
                       in_bndy%maxblocks_ns_rcv, in_bndy%nmsg_ns_rcv))

   allocate(snd_request(in_bndy%nmsg_ns_snd), &
            rcv_request(in_bndy%nmsg_ns_rcv), &
            snd_status(MPI_STATUS_SIZE,in_bndy%nmsg_ns_snd), &
            rcv_status(MPI_STATUS_SIZE,in_bndy%nmsg_ns_rcv))

!-----------------------------------------------------------------------
!
!  post receives
!
!-----------------------------------------------------------------------

   !call timer_start(bndy_2d_recv)
   do n=1,in_bndy%nmsg_ns_rcv

      bufsize = nx_block*(nghost+1)*in_bndy%nblocks_ns_rcv(n)

      call MPI_IRECV(buf_ns_rcv(1,1,1,n), bufsize, mpi_dbl,   &
                     in_bndy%ns_rcv_proc(n)-1,                &
                     mpitag_bndy_2d + in_bndy%ns_rcv_proc(n), &
                     in_bndy%communicator, rcv_request(n), ierr)
   end do
   !call timer_stop(bndy_2d_recv)

!-----------------------------------------------------------------------
!
!  fill send buffer and post sends
!
!-----------------------------------------------------------------------

   !call timer_start(bndy_2d_send)
   do n=1,in_bndy%nmsg_ns_snd

      bufsize = nx_block*(nghost+1)*in_bndy%nblocks_ns_snd(n)

      do i=1,in_bndy%nblocks_ns_snd(n)
         jb_src    = in_bndy%ns_src_add(2,i,n)
         je_src    = jb_src + nghost  ! nghost+1 rows needed for tripole
         src_block = in_bndy%ns_src_block(i,n)
         buf_ns_snd(:,:,i,n) = ARRAY(:,jb_src:je_src,src_block)
      end do

      call MPI_ISEND(buf_ns_snd(1,1,1,n), bufsize, mpi_dbl, &
                     in_bndy%ns_snd_proc(n)-1, &
                     mpitag_bndy_2d + my_task + 1, &
                     in_bndy%communicator, snd_request(n), ierr)
   end do
   !call timer_stop(bndy_2d_send)

!-----------------------------------------------------------------------
!
!  do local copies while waiting for messages to complete
!
!-----------------------------------------------------------------------

   if (allocated(tripole_dbuf)) tripole_dbuf = c0

   !call timer_start(bndy_2d_local)
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
   !call timer_stop(bndy_2d_local)

!-----------------------------------------------------------------------
!
!  wait for receives to finish and then unpack the recv buffer into
!  ghost cells
!
!-----------------------------------------------------------------------

   !call timer_start(bndy_2d_wait)
   call MPI_WAITALL(in_bndy%nmsg_ns_rcv, rcv_request, rcv_status, ierr)
   !call timer_stop(bndy_2d_wait)

   !call timer_start(bndy_2d_final)
   do n=1,in_bndy%nmsg_ns_rcv
   do k=1,in_bndy%nblocks_ns_rcv(n)
      dst_block = in_bndy%ns_dst_block(k,n)  ! dest block

      if (dst_block > 0) then  ! normal receive
         jb_dst = in_bndy%ns_dst_add(2,k,n)
         je_dst = jb_dst + nghost - 1

         ARRAY(:,jb_dst:je_dst,dst_block) = buf_ns_rcv(:,1:nghost,k,n)
      else ! northern tripole bndy: copy into global tripole buffer

         !*** determine start,end of physical domain for both
         !*** global buffer and local buffer

         ib_dst = in_bndy%ns_dst_add(1,k,n)
         ie_dst = ib_dst + (nx_block-2*nghost) - 1
         if (ie_dst > nx_global) ie_dst = nx_global
         ib_src = nghost + 1
         ie_src = ib_src + ie_dst - ib_dst
         if (src_block /= 0) then
            tripole_dbuf(ib_dst:ie_dst,:) = &
            buf_ns_rcv(ib_src:ie_src,:,k,n)
         endif
      endif
   end do
   end do
   !call timer_stop(bndy_2d_final)

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
            if (ib_dst == 0) ib_dst = nx_global
            xavg = p5*(abs(tripole_dbuf(i     ,nghost+1)) + &
                       abs(tripole_dbuf(ib_dst,nghost+1)))
            tripole_dghost(i     ,1) = sign(xavg, &
                                            tripole_dbuf(i,nghost+1))
            tripole_dghost(ib_dst,1) = sign(xavg, &
                                            tripole_dbuf(ib_dst,nghost+1))
         end do
         !*** catch nx_global point
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

      do n=1,in_bndy%nmsg_ns_rcv
      do k=1,in_bndy%nblocks_ns_rcv(n)
         dst_block = in_bndy%ns_dst_block(k,n)  ! dest block

         if (dst_block < 0) then
            dst_block = -dst_block

            jb_dst = in_bndy%ns_dst_add(2,k,n)
            je_dst = jb_dst + nghost ! last phys row incl for symmetry
            ib_src = in_bndy%ns_dst_add(3,k,n)
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

            ARRAY(ib_dst:ie_dst,jb_dst:je_dst,dst_block) = &
               tripole_dghost(ib_src:ie_src,:)
         endif


      end do
      end do

   endif

!-----------------------------------------------------------------------
!
!  wait for sends to complete and deallocate arrays
!
!-----------------------------------------------------------------------

   !call timer_start(bndy_2d_wait)
   call MPI_WAITALL(in_bndy%nmsg_ns_snd, snd_request, snd_status, ierr)
   !call timer_stop(bndy_2d_wait)

   deallocate(buf_ns_snd, buf_ns_rcv)
   deallocate(snd_request, rcv_request, snd_status, rcv_status)

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

! !USER:

   include 'mpif.h'   ! MPI Fortran include file

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
      bufsize,                     &! buffer size for send/recv buffers
      xoffset, yoffset,            &! address shifts for tripole
      isign,                       &! sign factor for tripole grids
      ierr                          ! MPI error flag

   integer (int_kind), dimension(:), allocatable :: &
      snd_request,              &! MPI request ids
      rcv_request                ! MPI request ids

   integer (int_kind), dimension(:,:), allocatable :: &
      snd_status,               &! MPI status flags
      rcv_status                 ! MPI status flags

   real (r4), dimension(:,:,:,:), allocatable :: &
      buf_ew_snd,       &! message buffer for east-west sends
      buf_ew_rcv,       &! message buffer for east-west recvs
      buf_ns_snd,       &! message buffer for north-south sends
      buf_ns_rcv         ! message buffer for north-south recvs

   real (r4) :: &
      xavg               ! scalar for enforcing symmetry at U pts

!-----------------------------------------------------------------------
!
!  allocate buffers for east-west sends and receives
!
!-----------------------------------------------------------------------

   allocate(buf_ew_snd(nghost, ny_block, &
                       in_bndy%maxblocks_ew_snd, in_bndy%nmsg_ew_snd),&
            buf_ew_rcv(nghost, ny_block, &
                       in_bndy%maxblocks_ew_rcv, in_bndy%nmsg_ew_rcv))

   allocate(snd_request(in_bndy%nmsg_ew_snd), &
            rcv_request(in_bndy%nmsg_ew_rcv), &
            snd_status(MPI_STATUS_SIZE,in_bndy%nmsg_ew_snd), &
            rcv_status(MPI_STATUS_SIZE,in_bndy%nmsg_ew_rcv))

   if (allocated(tripole_rbuf)) nx_global = size(tripole_rbuf,dim=1)

!-----------------------------------------------------------------------
!
!  post receives
!
!-----------------------------------------------------------------------

   do n=1,in_bndy%nmsg_ew_rcv

      bufsize = ny_block*nghost*in_bndy%nblocks_ew_rcv(n)

      call MPI_IRECV(buf_ew_rcv(1,1,1,n), bufsize, mpi_real,  &
                     in_bndy%ew_rcv_proc(n)-1,                &
                     mpitag_bndy_2d + in_bndy%ew_rcv_proc(n), &
                     in_bndy%communicator, rcv_request(n), ierr)
   end do

!-----------------------------------------------------------------------
!
!  fill send buffer and post sends
!
!-----------------------------------------------------------------------

   do n=1,in_bndy%nmsg_ew_snd

      bufsize = ny_block*nghost*in_bndy%nblocks_ew_snd(n)

      do i=1,in_bndy%nblocks_ew_snd(n)
         ib_src    = in_bndy%ew_src_add(1,i,n)
         ie_src    = ib_src + nghost - 1
         src_block = in_bndy%ew_src_block(i,n)
         buf_ew_snd(:,:,i,n) = ARRAY(ib_src:ie_src,:,src_block)
      end do

      call MPI_ISEND(buf_ew_snd(1,1,1,n), bufsize, mpi_real, &
                     in_bndy%ew_snd_proc(n)-1, &
                     mpitag_bndy_2d + my_task + 1, &
                     in_bndy%communicator, snd_request(n), ierr)
   end do

!-----------------------------------------------------------------------
!
!  do local copies while waiting for messages to complete
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
!  wait for receives to finish and then unpack the recv buffer into
!  ghost cells
!
!-----------------------------------------------------------------------

   call MPI_WAITALL(in_bndy%nmsg_ew_rcv, rcv_request, rcv_status, ierr)

   do n=1,in_bndy%nmsg_ew_rcv
   do k=1,in_bndy%nblocks_ew_rcv(n)
      dst_block = in_bndy%ew_dst_block(k,n)

      ib_dst = in_bndy%ew_dst_add(1,k,n)
      ie_dst = ib_dst + nghost - 1

      ARRAY(ib_dst:ie_dst,:,dst_block) = buf_ew_rcv(:,:,k,n)
   end do
   end do

!-----------------------------------------------------------------------
!
!  wait for sends to complete and deallocate arrays
!
!-----------------------------------------------------------------------

   call MPI_WAITALL(in_bndy%nmsg_ew_snd, snd_request, snd_status, ierr)

   deallocate(buf_ew_snd, buf_ew_rcv)
   deallocate(snd_request, rcv_request, snd_status, rcv_status)

!-----------------------------------------------------------------------
!
!  now exchange north-south boundary info
!
!-----------------------------------------------------------------------

   allocate(buf_ns_snd(nx_block, nghost+1, &
                       in_bndy%maxblocks_ns_snd, in_bndy%nmsg_ns_snd),&
            buf_ns_rcv(nx_block, nghost+1, &
                       in_bndy%maxblocks_ns_rcv, in_bndy%nmsg_ns_rcv))

   allocate(snd_request(in_bndy%nmsg_ns_snd), &
            rcv_request(in_bndy%nmsg_ns_rcv), &
            snd_status(MPI_STATUS_SIZE,in_bndy%nmsg_ns_snd), &
            rcv_status(MPI_STATUS_SIZE,in_bndy%nmsg_ns_rcv))

!-----------------------------------------------------------------------
!
!  post receives
!
!-----------------------------------------------------------------------

   do n=1,in_bndy%nmsg_ns_rcv

      bufsize = nx_block*(nghost+1)*in_bndy%nblocks_ns_rcv(n)

      call MPI_IRECV(buf_ns_rcv(1,1,1,n), bufsize, mpi_real,  &
                     in_bndy%ns_rcv_proc(n)-1,                &
                     mpitag_bndy_2d + in_bndy%ns_rcv_proc(n), &
                     in_bndy%communicator, rcv_request(n), ierr)
   end do

!-----------------------------------------------------------------------
!
!  fill send buffer and post sends
!
!-----------------------------------------------------------------------

   do n=1,in_bndy%nmsg_ns_snd

      bufsize = nx_block*(nghost+1)*in_bndy%nblocks_ns_snd(n)

      do i=1,in_bndy%nblocks_ns_snd(n)
         jb_src    = in_bndy%ns_src_add(2,i,n)
         je_src    = jb_src + nghost  ! nghost+1 rows needed for tripole
         src_block = in_bndy%ns_src_block(i,n)
         buf_ns_snd(:,:,i,n) = ARRAY(:,jb_src:je_src,src_block)
      end do

      call MPI_ISEND(buf_ns_snd(1,1,1,n), bufsize, mpi_real, &
                     in_bndy%ns_snd_proc(n)-1, &
                     mpitag_bndy_2d + my_task + 1, &
                     in_bndy%communicator, snd_request(n), ierr)
   end do

!-----------------------------------------------------------------------
!
!  do local copies while waiting for messages to complete
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
!  wait for receives to finish and then unpack the recv buffer into
!  ghost cells
!
!-----------------------------------------------------------------------

   call MPI_WAITALL(in_bndy%nmsg_ns_rcv, rcv_request, rcv_status, ierr)

   do n=1,in_bndy%nmsg_ns_rcv
   do k=1,in_bndy%nblocks_ns_rcv(n)
      dst_block = in_bndy%ns_dst_block(k,n)  ! dest block

      if (dst_block > 0) then  ! normal receive
         jb_dst = in_bndy%ns_dst_add(2,k,n)
         je_dst = jb_dst + nghost - 1

         ARRAY(:,jb_dst:je_dst,dst_block) = buf_ns_rcv(:,1:nghost,k,n)
      else ! northern tripole bndy: copy into global tripole buffer

         !*** determine start,end of physical domain for both
         !*** global buffer and local buffer

         ib_dst = in_bndy%ns_dst_add(1,k,n)
         ie_dst = ib_dst + (nx_block-2*nghost) - 1
         if (ie_dst > nx_global) ie_dst = nx_global
         ib_src = nghost + 1
         ie_src = ib_src + ie_dst - ib_dst
         if (src_block /= 0) then
            tripole_rbuf(ib_dst:ie_dst,:) = &
            buf_ns_rcv(ib_src:ie_src,:,k,n)
         endif
      endif
   end do
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
            if (ib_dst == 0) ib_dst = nx_global
            xavg = p5*(abs(tripole_rbuf(i     ,nghost+1)) + &
                       abs(tripole_rbuf(ib_dst,nghost+1)))
            tripole_rghost(i     ,1) = sign(xavg, &
                                            tripole_rbuf(i,nghost+1))
            tripole_rghost(ib_dst,1) = sign(xavg, &
                                            tripole_rbuf(ib_dst,nghost+1))
         end do
         !*** catch nx_global point
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

      do n=1,in_bndy%nmsg_ns_rcv
      do k=1,in_bndy%nblocks_ns_rcv(n)
         dst_block = in_bndy%ns_dst_block(k,n)  ! dest block

         if (dst_block < 0) then
            dst_block = -dst_block

            jb_dst = in_bndy%ns_dst_add(2,k,n)
            je_dst = jb_dst + nghost ! last phys row incl for symmetry
            ib_src = in_bndy%ns_dst_add(3,k,n)
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

            ARRAY(ib_dst:ie_dst,jb_dst:je_dst,dst_block) = &
               tripole_rghost(ib_src:ie_src,:)
         endif


      end do
      end do

   endif

!-----------------------------------------------------------------------
!
!  wait for sends to complete and deallocate arrays
!
!-----------------------------------------------------------------------

   call MPI_WAITALL(in_bndy%nmsg_ns_snd, snd_request, snd_status, ierr)

   deallocate(buf_ns_snd, buf_ns_rcv)
   deallocate(snd_request, rcv_request, snd_status, rcv_status)

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

! !USER:

   include 'mpif.h'   ! MPI Fortran include file

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
      bufsize,                     &! buffer size for send/recv buffers
      xoffset, yoffset,            &! address shifts for tripole
      isign,                       &! sign factor for tripole grids
      ierr                          ! MPI error flag

   integer (int_kind), dimension(:), allocatable :: &
      snd_request,              &! MPI request ids
      rcv_request                ! MPI request ids

   integer (int_kind), dimension(:,:), allocatable :: &
      snd_status,               &! MPI status flags
      rcv_status                 ! MPI status flags

   integer (int_kind), dimension(:,:,:,:), allocatable :: &
      buf_ew_snd,       &! message buffer for east-west sends
      buf_ew_rcv,       &! message buffer for east-west recvs
      buf_ns_snd,       &! message buffer for north-south sends
      buf_ns_rcv         ! message buffer for north-south recvs

   integer (int_kind) :: &
      xavg               ! scalar for enforcing symmetry at U pts

!-----------------------------------------------------------------------
!
!  allocate buffers for east-west sends and receives
!
!-----------------------------------------------------------------------

   allocate(buf_ew_snd(nghost, ny_block, &
                       in_bndy%maxblocks_ew_snd, in_bndy%nmsg_ew_snd),&
            buf_ew_rcv(nghost, ny_block, &
                       in_bndy%maxblocks_ew_rcv, in_bndy%nmsg_ew_rcv))

   allocate(snd_request(in_bndy%nmsg_ew_snd), &
            rcv_request(in_bndy%nmsg_ew_rcv), &
            snd_status(MPI_STATUS_SIZE,in_bndy%nmsg_ew_snd), &
            rcv_status(MPI_STATUS_SIZE,in_bndy%nmsg_ew_rcv))

   if (allocated(tripole_ibuf)) nx_global = size(tripole_ibuf,dim=1)

!-----------------------------------------------------------------------
!
!  post receives
!
!-----------------------------------------------------------------------

   do n=1,in_bndy%nmsg_ew_rcv

      bufsize = ny_block*nghost*in_bndy%nblocks_ew_rcv(n)

      call MPI_IRECV(buf_ew_rcv(1,1,1,n), bufsize, mpi_integer, &
                     in_bndy%ew_rcv_proc(n)-1,                  &
                     mpitag_bndy_2d + in_bndy%ew_rcv_proc(n),   &
                     in_bndy%communicator, rcv_request(n), ierr)
   end do

!-----------------------------------------------------------------------
!
!  fill send buffer and post sends
!
!-----------------------------------------------------------------------

   do n=1,in_bndy%nmsg_ew_snd

      bufsize = ny_block*nghost*in_bndy%nblocks_ew_snd(n)

      do i=1,in_bndy%nblocks_ew_snd(n)
         ib_src    = in_bndy%ew_src_add(1,i,n)
         ie_src    = ib_src + nghost - 1
         src_block = in_bndy%ew_src_block(i,n)
         buf_ew_snd(:,:,i,n) = ARRAY(ib_src:ie_src,:,src_block)
      end do

      call MPI_ISEND(buf_ew_snd(1,1,1,n), bufsize, mpi_integer, &
                     in_bndy%ew_snd_proc(n)-1, &
                     mpitag_bndy_2d + my_task + 1, &
                     in_bndy%communicator, snd_request(n), ierr)
   end do

!-----------------------------------------------------------------------
!
!  do local copies while waiting for messages to complete
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
!  wait for receives to finish and then unpack the recv buffer into
!  ghost cells
!
!-----------------------------------------------------------------------

   call MPI_WAITALL(in_bndy%nmsg_ew_rcv, rcv_request, rcv_status, ierr)

   do n=1,in_bndy%nmsg_ew_rcv
   do k=1,in_bndy%nblocks_ew_rcv(n)
      dst_block = in_bndy%ew_dst_block(k,n)

      ib_dst = in_bndy%ew_dst_add(1,k,n)
      ie_dst = ib_dst + nghost - 1

      ARRAY(ib_dst:ie_dst,:,dst_block) = buf_ew_rcv(:,:,k,n)
   end do
   end do

!-----------------------------------------------------------------------
!
!  wait for sends to complete and deallocate arrays
!
!-----------------------------------------------------------------------

   call MPI_WAITALL(in_bndy%nmsg_ew_snd, snd_request, snd_status, ierr)

   deallocate(buf_ew_snd, buf_ew_rcv)
   deallocate(snd_request, rcv_request, snd_status, rcv_status)

!-----------------------------------------------------------------------
!
!  now exchange north-south boundary info
!
!-----------------------------------------------------------------------

   allocate(buf_ns_snd(nx_block, nghost+1, &
                       in_bndy%maxblocks_ns_snd, in_bndy%nmsg_ns_snd),&
            buf_ns_rcv(nx_block, nghost+1, &
                       in_bndy%maxblocks_ns_rcv, in_bndy%nmsg_ns_rcv))

   allocate(snd_request(in_bndy%nmsg_ns_snd), &
            rcv_request(in_bndy%nmsg_ns_rcv), &
            snd_status(MPI_STATUS_SIZE,in_bndy%nmsg_ns_snd), &
            rcv_status(MPI_STATUS_SIZE,in_bndy%nmsg_ns_rcv))

!-----------------------------------------------------------------------
!
!  post receives
!
!-----------------------------------------------------------------------

   do n=1,in_bndy%nmsg_ns_rcv

      bufsize = nx_block*(nghost+1)*in_bndy%nblocks_ns_rcv(n)

      call MPI_IRECV(buf_ns_rcv(1,1,1,n), bufsize, mpi_integer,   &
                     in_bndy%ns_rcv_proc(n)-1,                &
                     mpitag_bndy_2d + in_bndy%ns_rcv_proc(n), &
                     in_bndy%communicator, rcv_request(n), ierr)
   end do

!-----------------------------------------------------------------------
!
!  fill send buffer and post sends
!
!-----------------------------------------------------------------------

   do n=1,in_bndy%nmsg_ns_snd

      bufsize = nx_block*(nghost+1)*in_bndy%nblocks_ns_snd(n)

      do i=1,in_bndy%nblocks_ns_snd(n)
         jb_src    = in_bndy%ns_src_add(2,i,n)
         je_src    = jb_src + nghost  ! nghost+1 rows needed for tripole
         src_block = in_bndy%ns_src_block(i,n)
         buf_ns_snd(:,:,i,n) = ARRAY(:,jb_src:je_src,src_block)
      end do

      call MPI_ISEND(buf_ns_snd(1,1,1,n), bufsize, mpi_integer, &
                     in_bndy%ns_snd_proc(n)-1, &
                     mpitag_bndy_2d + my_task + 1, &
                     in_bndy%communicator, snd_request(n), ierr)
   end do

!-----------------------------------------------------------------------
!
!  do local copies while waiting for messages to complete
!
!-----------------------------------------------------------------------

   if (allocated(tripole_ibuf)) tripole_ibuf = c0

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
!  wait for receives to finish and then unpack the recv buffer into
!  ghost cells
!
!-----------------------------------------------------------------------

   call MPI_WAITALL(in_bndy%nmsg_ns_rcv, rcv_request, rcv_status, ierr)

   do n=1,in_bndy%nmsg_ns_rcv
   do k=1,in_bndy%nblocks_ns_rcv(n)
      dst_block = in_bndy%ns_dst_block(k,n)  ! dest block

      if (dst_block > 0) then  ! normal receive
         jb_dst = in_bndy%ns_dst_add(2,k,n)
         je_dst = jb_dst + nghost - 1

         ARRAY(:,jb_dst:je_dst,dst_block) = buf_ns_rcv(:,1:nghost,k,n)
      else ! northern tripole bndy: copy into global tripole buffer

         !*** determine start,end of physical domain for both
         !*** global buffer and local buffer

         ib_dst = in_bndy%ns_dst_add(1,k,n)
         ie_dst = ib_dst + (nx_block-2*nghost) - 1
         if (ie_dst > nx_global) ie_dst = nx_global
         ib_src = nghost + 1
         ie_src = ib_src + ie_dst - ib_dst
         if (src_block /= 0) then
            tripole_ibuf(ib_dst:ie_dst,:) = &
            buf_ns_rcv(ib_src:ie_src,:,k,n)
         endif
      endif
   end do
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
            if (ib_dst == 0) ib_dst = nx_global
            xavg = p5*(abs(tripole_ibuf(i     ,nghost+1)) + &
                       abs(tripole_ibuf(ib_dst,nghost+1)))
            tripole_ighost(i     ,1) = sign(xavg, &
                                          tripole_ibuf(i,nghost+1))
            tripole_ighost(ib_dst,1) = sign(xavg, &
                                          tripole_ibuf(ib_dst,nghost+1))
         end do
         !*** catch nx_global point
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

      do n=1,in_bndy%nmsg_ns_rcv
      do k=1,in_bndy%nblocks_ns_rcv(n)
         dst_block = in_bndy%ns_dst_block(k,n)  ! dest block

         if (dst_block < 0) then
            dst_block = -dst_block

            jb_dst = in_bndy%ns_dst_add(2,k,n)
            je_dst = jb_dst + nghost ! last phys row incl for symmetry
            ib_src = in_bndy%ns_dst_add(3,k,n)
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

            ARRAY(ib_dst:ie_dst,jb_dst:je_dst,dst_block) = &
               tripole_ighost(ib_src:ie_src,:)
         endif


      end do
      end do

   endif

!-----------------------------------------------------------------------
!
!  wait for sends to complete and deallocate arrays
!
!-----------------------------------------------------------------------

   !call timer_start(bndy_2d_wait)
   call MPI_WAITALL(in_bndy%nmsg_ns_snd, snd_request, snd_status, ierr)
   !call timer_stop(bndy_2d_wait)

   deallocate(buf_ns_snd, buf_ns_rcv)
   deallocate(snd_request, rcv_request, snd_status, rcv_status)

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
!BOP
! !IROUTINE: increment_message_counter
! !INTERFACE:

   subroutine increment_message_counter(snd_counter, rcv_counter, &
                                        src_proc, dst_proc)

! !DESCRIPTION:
!  This is a utility routine to increment the arrays for counting
!  whether messages are required.  It is used only for creating
!  boundary structures for updating ghost cells.

! !REVISION HISTORY:
!  Same as module.

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
      src_proc,              & ! source processor for communication
      dst_proc                 ! destination processor for communication

! !INPUT/OUTPUT PARAMETERS:

   integer (int_kind), dimension(:), intent(inout) :: &
      snd_counter,       &! array for counting messages to be sent
      rcv_counter         ! array for counting messages to be received

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  if destination all land (proc = 0), then no send is necessary, 
!  so do the rest only for proc /= 0
!
!-----------------------------------------------------------------------

   if (dst_proc /= 0) then

      !*** if the current processor is the source, must send
      !*** data (local copy if dst_proc = src_proc)

      if (src_proc == my_task + 1) snd_counter(dst_proc) = &
                                   snd_counter(dst_proc) + 1

      !*** if the current processor is the destination, must
      !*** receive data (local copy if dst_proc = src_proc)

      if (dst_proc == my_task + 1) then

         if (src_proc /= 0) then  
            !*** the source block has ocean points so
            !*** increment the number of messages from
            !*** the source process

            rcv_counter(src_proc) = rcv_counter(src_proc) + 1

         else
            !*** the source block has no ocean points so
            !*** count this as a local copy in order to
            !*** fill ghost cells with zeroes

            rcv_counter(dst_proc) = rcv_counter(dst_proc) + 1

         endif
      endif
   endif

!-----------------------------------------------------------------------
!EOC

   end subroutine increment_message_counter

!***********************************************************************

end module boundary

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
