!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
!BOP
! !MODULE: gather_scatter

 module gather_scatter

! !DESCRIPTION:
!  This module contains routines for gathering data to a single
!  processor from a distributed array, scattering data from a
!  single processor to a distributed array and changing distribution
!  of blocks of data (eg from baroclinic to barotropic and back).
!
! !REVISION HISTORY:
!  CVS: $Id: gather_scatter.F90,v 1.7 2003/12/23 22:35:04 pwjones Exp $
!  CVS: $Name: POP_2_0_1 $

! !USES:

   use kinds_mod
   use communicate
   use constants
   use blocks
   use distribution
   use domain
   use domain_size
   use exit_mod

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public :: gather_global,      &
             scatter_global,     &
             redistribute_blocks

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  overload module functions
!
!-----------------------------------------------------------------------

   interface gather_global
     module procedure gather_global_dbl,  &
                      gather_global_real, &
                      gather_global_int
   end interface

   interface scatter_global
     module procedure scatter_global_dbl,  &
                      scatter_global_real, &
                      scatter_global_int
   end interface

   interface redistribute_blocks
     module procedure redistribute_blocks_dbl,  &
                      redistribute_blocks_real, &
                      redistribute_blocks_int
   end interface

!-----------------------------------------------------------------------
!
!  module variables
!
!-----------------------------------------------------------------------

!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: gather_global
! !INTERFACE:

 subroutine gather_global_dbl(ARRAY_G, ARRAY, dst_task, src_dist)

! !DESCRIPTION:
!  This subroutine gathers a distributed array to a global-sized
!  array on the processor dst_task.
!
! !REVISION HISTORY:
!  same as module
!
! !REMARKS:
!  This is the specific inteface for double precision arrays
!  corresponding to the generic interface gather_global.  It is shown
!  to provide information on the generic interface (the generic
!  interface is identical, but chooses a specific inteface based
!  on the data type of the input argument).

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
     dst_task   ! task to which array should be gathered

   type (distrb), intent(in) :: &
     src_dist   ! distribution of blocks in the source array

   real (r8), dimension(:,:,:), intent(in) :: &
     ARRAY      ! array containing horizontal slab of distributed field

! !OUTPUT PARAMETERS:

   real (r8), dimension(:,:), intent(inout) :: &
     ARRAY_G    ! array containing global horizontal field on dst_task

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
     i,j,n          ,&! dummy loop counters
     src_block        ! local block index in source distribution

   type (block) :: &
     this_block  ! block info for current block

!-----------------------------------------------------------------------
!
!  copy local array into block decomposition
!
!-----------------------------------------------------------------------

   do n=1,nblocks_tot

      this_block = get_block(n,n)

      !*** copy local blocks

      if (src_dist%proc(n) /= 0) then

         do j=this_block%jb,this_block%je
         do i=this_block%ib,this_block%ie
            ARRAY_G(this_block%i_glob(i), &
                    this_block%j_glob(j)) = &
                  ARRAY(i,j,src_dist%local_block(n))
         end do
         end do

      else !*** fill land blocks with zeroes

         do j=this_block%jb,this_block%je
         do i=this_block%ib,this_block%ie
            ARRAY_G(this_block%i_glob(i), &
                    this_block%j_glob(j)) = c0
         end do
         end do

      endif

   end do

!-----------------------------------------------------------------------

 end subroutine gather_global_dbl

!***********************************************************************

 subroutine gather_global_real(ARRAY_G, ARRAY, dst_task, src_dist)

!-----------------------------------------------------------------------
!
!  This subroutine gathers a distributed array to a global-sized
!  array on the processor dst_task.
!
!-----------------------------------------------------------------------
!-----------------------------------------------------------------------
!
!  input variables
!
!-----------------------------------------------------------------------

   integer (int_kind), intent(in) :: &
     dst_task       ! task to which array should be gathered

   type (distrb), intent(in) :: &
     src_dist       ! distribution of blocks in the source array

   real (r4), dimension(:,:,:), intent(in) :: &
     ARRAY          ! array containing distributed field

!-----------------------------------------------------------------------
!
!  output variables
!
!-----------------------------------------------------------------------

   real (r4), dimension(:,:), intent(inout) :: &
     ARRAY_G        ! array containing global field on dst_task

!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      i,j,n             ,&! dummy loop counters
      src_block           ! local block index in source distribution

   type (block) :: &
      this_block   ! block info for current block

!-----------------------------------------------------------------------
!
!  copy local array into block decomposition
!
!-----------------------------------------------------------------------

   do n=1,nblocks_tot

      this_block = get_block(n,n)

      !*** copy local blocks

      if (src_dist%proc(n) /= 0) then

         do j=this_block%jb,this_block%je
         do i=this_block%ib,this_block%ie
            ARRAY_G(this_block%i_glob(i), &
                    this_block%j_glob(j)) = &
                  ARRAY(i,j,src_dist%local_block(n))
         end do
         end do

      else !*** fill land blocks with zeroes

         do j=this_block%jb,this_block%je
         do i=this_block%ib,this_block%ie
            ARRAY_G(this_block%i_glob(i), &
                    this_block%j_glob(j)) = c0
         end do
         end do

      endif

   end do

!-----------------------------------------------------------------------

 end subroutine gather_global_real

!***********************************************************************

 subroutine gather_global_int(ARRAY_G, ARRAY, dst_task, src_dist)

!-----------------------------------------------------------------------
!
!  This subroutine gathers a distributed array to a global-sized
!  array on the processor dst_task.
!
!-----------------------------------------------------------------------
!-----------------------------------------------------------------------
!
!  input variables
!
!-----------------------------------------------------------------------

   integer (int_kind), intent(in) :: &
     dst_task       ! task to which array should be gathered

   type (distrb), intent(in) :: &
     src_dist       ! distribution of blocks in the source array

   integer (int_kind), dimension(:,:,:), intent(in) :: &
     ARRAY          ! array containing distributed field

!-----------------------------------------------------------------------
!
!  output variables
!
!-----------------------------------------------------------------------

   integer (int_kind), dimension(:,:), intent(inout) :: &
     ARRAY_G        ! array containing global field on dst_task

!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      i,j,n,             &! dummy loop counters
      src_block           ! local block index in source distribution

   type (block) :: &
      this_block    ! block info for current block

!-----------------------------------------------------------------------
!
!  copy local array into block decomposition
!
!-----------------------------------------------------------------------

   do n=1,nblocks_tot

      this_block = get_block(n,n)

      !*** copy local blocks

      if (src_dist%proc(n) /= 0) then

         do j=this_block%jb,this_block%je
         do i=this_block%ib,this_block%ie
            ARRAY_G(this_block%i_glob(i), &
                    this_block%j_glob(j)) = &
                  ARRAY(i,j,src_dist%local_block(n))
         end do
         end do

      else !*** fill land blocks with zeroes

         do j=this_block%jb,this_block%je
         do i=this_block%ib,this_block%ie
            ARRAY_G(this_block%i_glob(i), &
                    this_block%j_glob(j)) = c0
         end do
         end do

      endif

   end do

!-----------------------------------------------------------------------

 end subroutine gather_global_int

!EOC
!***********************************************************************
!BOP
! !IROUTINE: scatter_global
! !INTERFACE:

 subroutine scatter_global_dbl(ARRAY, ARRAY_G, src_task, dst_dist, &
                               field_loc, field_type)

! !DESCRIPTION:
!  This subroutine scatters a global-sized array on the processor
!  src\_task to a distribution of blocks given by dst\_dist.
!
! !REVISION HISTORY:
!  same as module
!
! !REMARKS:
!  This is the specific interface for double precision arrays
!  corresponding to the generic interface scatter_global.  It is shown
!  to provide information on the generic interface (the generic
!  interface is identical, but chooses a specific interface based
!  on the data type of the input argument).

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
     src_task       ! task from which array should be scattered

   type (distrb), intent(in) :: &
     dst_dist       ! distribution of resulting blocks

   real (r8), dimension(:,:), intent(in) :: &
     ARRAY_G        ! array containing global field on src_task

   integer (int_kind), intent(in) :: &
      field_type,               &! id for type of field (scalar, vector, angle)
      field_loc                  ! id for location on horizontal grid
                                 !  (center, NEcorner, Nface, Eface)

! !OUTPUT PARAMETERS:

   real (r8), dimension(:,:,:), intent(inout) :: &
     ARRAY          ! array containing distributed field

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
     i,j,n,bid,          &! dummy loop indices
     isrc, jsrc,         &! source addresses
     xoffset, yoffset,   &! offsets for tripole bounday conditions
     isign,              &! sign factor for tripole boundary conditions
     dst_block            ! local block index in dest distribution

   type (block) :: &
     this_block  ! block info for current block

!-----------------------------------------------------------------------
!
!  initialize return array to zero and set up tripole quantities
!
!-----------------------------------------------------------------------

   ARRAY = c0

   select case (field_loc)
   case (field_loc_center)   ! cell center location
      xoffset = 1
      yoffset = 1
   case (field_loc_NEcorner)   ! cell corner (velocity) location
      xoffset = 0
      yoffset = 0
   case (field_loc_Eface)   ! cell center location
      xoffset = 0
      yoffset = 1
   case (field_loc_Nface)   ! cell corner (velocity) location
      xoffset = 1
      yoffset = 0
   case (field_loc_noupdate) ! ghost cells not needed - use cell center
      xoffset = 1
      yoffset = 1
   end select

   select case (field_type)
   case (field_type_scalar)
      isign =  1
   case (field_type_vector)
      isign = -1
   case (field_type_angle)
      isign = -1
   case (field_type_noupdate) ! ghost cells not needed - use cell center
      isign =  1
   case default
      call exit_POP(sigAbort, 'Unknown field type in scatter')
   end select

!-----------------------------------------------------------------------
!
!  copy blocks of global array into local block distribution
!
!-----------------------------------------------------------------------

   do n=1,nblocks_tot

      if (dst_dist%proc(n) /= 0) then

         this_block = get_block(n,n)
         dst_block  = dst_dist%local_block(n)

         !*** if this is an interior block, then there is no
         !*** padding or update checking required

         if (this_block%iblock > 1         .and. &
             this_block%iblock < nblocks_x .and. &
             this_block%jblock > 1         .and. &
             this_block%jblock < nblocks_y) then

            do j=1,ny_block
            do i=1,nx_block
               ARRAY(i,j,dst_block) = ARRAY_G(this_block%i_glob(i),&
                                              this_block%j_glob(j))
            end do
            end do

         !*** if this is an edge block but not a northern edge
         !*** we only need to check for closed boundaries and
         !*** padding (global index = 0)

         else if (this_block%jblock /= nblocks_y) then

            do j=1,ny_block
               if (this_block%j_glob(j) /= 0) then
                  do i=1,nx_block
                     if (this_block%i_glob(i) /= 0) then
                        ARRAY(i,j,dst_block) = ARRAY_G(this_block%i_glob(i),&
                                                       this_block%j_glob(j))
                     endif
                  end do
               endif
            end do

         !*** if this is a northern edge block, we need to check
         !*** for and properly deal with tripole boundaries

         else

            do j=1,ny_block
               if (this_block%j_glob(j) > 0) then ! normal boundary

                  do i=1,nx_block
                     if (this_block%i_glob(i) /= 0) then
                        ARRAY(i,j,dst_block) = ARRAY_G(this_block%i_glob(i),&
                                                       this_block%j_glob(j))
                     endif
                  end do

               else if (this_block%j_glob(j) < 0) then  ! tripole

                  jsrc = ny_global + yoffset + &
                         (this_block%j_glob(j) + ny_global)
                  do i=1,nx_block
                     if (this_block%i_glob(i) /= 0) then
                        isrc = nx_global + xoffset - this_block%i_glob(i)
                        if (isrc < 1) isrc = isrc + nx_global
                        if (isrc > nx_global) isrc = isrc - nx_global
                        ARRAY(i,j,dst_block) = ARRAY_G(isrc,jsrc)
                     endif
                  end do

               endif
            end do

         endif
      endif ! dst block not land
   end do  ! block loop

!-----------------------------------------------------------------------

 end subroutine scatter_global_dbl

!***********************************************************************

 subroutine scatter_global_real(ARRAY, ARRAY_G, src_task, dst_dist, &
                                field_loc, field_type)

!-----------------------------------------------------------------------
!
!  This subroutine scatters a global-sized array on the processor
!  src\_task to a distribution of blocks given by dst\_dist.
!
!-----------------------------------------------------------------------
!-----------------------------------------------------------------------
!
!  input variables
!
!-----------------------------------------------------------------------

   integer (int_kind), intent(in) :: &
     src_task       ! task from which array should be scattered

   type (distrb), intent(in) :: &
     dst_dist       ! distribution of resulting blocks

   real (r4), dimension(:,:), intent(in) :: &
     ARRAY_G        ! array containing global field on src_task

   integer (int_kind), intent(in) :: &
      field_type,               &! id for type of field (scalar, vector, angle)
      field_loc                  ! id for location on horizontal grid
                                 !  (center, NEcorner, Nface, Eface)

!-----------------------------------------------------------------------
!
!  output variables
!
!-----------------------------------------------------------------------

   real (r4), dimension(:,:,:), intent(inout) :: &
     ARRAY          ! array containing distributed field

!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
     i,j,n,bid,          &! dummy loop indices
     isrc, jsrc,         &! source addresses
     xoffset, yoffset,   &! offsets for tripole bounday conditions
     isign,              &! sign factor for tripole boundary conditions
     dst_block            ! local block index in dest distribution

   type (block) :: &
     this_block  ! block info for current block

!-----------------------------------------------------------------------
!
!  initialize return array to zero and set up tripole quantities
!
!-----------------------------------------------------------------------

   ARRAY = c0

   select case (field_loc)
   case (field_loc_center)   ! cell center location
      xoffset = 1
      yoffset = 1
   case (field_loc_NEcorner)   ! cell corner (velocity) location
      xoffset = 0
      yoffset = 0
   case (field_loc_Eface)   ! cell center location
      xoffset = 0
      yoffset = 1
   case (field_loc_Nface)   ! cell corner (velocity) location
      xoffset = 1
      yoffset = 0
   case (field_loc_noupdate) ! ghost cells not needed - use cell center
      xoffset = 1
      yoffset = 1
   end select

   select case (field_type)
   case (field_type_scalar)
      isign =  1
   case (field_type_vector)
      isign = -1
   case (field_type_angle)
      isign = -1
   case (field_type_noupdate) ! ghost cells not needed - use cell center
      isign =  1
   case default
      call exit_POP(sigAbort, 'Unknown field type in scatter')
   end select

!-----------------------------------------------------------------------
!
!  copy blocks of global array into local block distribution
!
!-----------------------------------------------------------------------

   do n=1,nblocks_tot

      if (dst_dist%proc(n) /= 0) then

         this_block = get_block(n,n)
         dst_block  = dst_dist%local_block(n)

         !*** if this is an interior block, then there is no
         !*** padding or update checking required

         if (this_block%iblock > 1         .and. &
             this_block%iblock < nblocks_x .and. &
             this_block%jblock > 1         .and. &
             this_block%jblock < nblocks_y) then

            do j=1,ny_block
            do i=1,nx_block
               ARRAY(i,j,dst_block) = ARRAY_G(this_block%i_glob(i),&
                                              this_block%j_glob(j))
            end do
            end do

         !*** if this is an edge block but not a northern edge
         !*** we only need to check for closed boundaries and
         !*** padding (global index = 0)

         else if (this_block%jblock /= nblocks_y) then

            do j=1,ny_block
               if (this_block%j_glob(j) /= 0) then
                  do i=1,nx_block
                     if (this_block%i_glob(i) /= 0) then
                        ARRAY(i,j,dst_block) = ARRAY_G(this_block%i_glob(i),&
                                                       this_block%j_glob(j))
                     endif
                  end do
               endif
            end do

         !*** if this is a northern edge block, we need to check
         !*** for and properly deal with tripole boundaries

         else

            do j=1,ny_block
               if (this_block%j_glob(j) > 0) then ! normal boundary

                  do i=1,nx_block
                     if (this_block%i_glob(i) /= 0) then
                        ARRAY(i,j,dst_block) = ARRAY_G(this_block%i_glob(i),&
                                                       this_block%j_glob(j))
                     endif
                  end do

               else if (this_block%j_glob(j) < 0) then  ! tripole

                  jsrc = ny_global + yoffset + &
                         (this_block%j_glob(j) + ny_global)
                  do i=1,nx_block
                     if (this_block%i_glob(i) /= 0) then
                        isrc = nx_global + xoffset - this_block%i_glob(i)
                        if (isrc < 1) isrc = isrc + nx_global
                        if (isrc > nx_global) isrc = isrc - nx_global
                        ARRAY(i,j,dst_block) = ARRAY_G(isrc,jsrc)
                     endif
                  end do

               endif
            end do

         endif
      endif ! dst block not land
   end do  ! block loop

!-----------------------------------------------------------------------

 end subroutine scatter_global_real

!***********************************************************************

 subroutine scatter_global_int(ARRAY, ARRAY_G, src_task, dst_dist, &
                               field_loc, field_type)

!-----------------------------------------------------------------------
!
!  This subroutine scatters a global-sized array on the processor
!  src\_task to a distribution of blocks given by dst\_dist.
!
!-----------------------------------------------------------------------
!-----------------------------------------------------------------------
!
!  input variables
!
!-----------------------------------------------------------------------

   integer (int_kind), intent(in) :: &
     src_task       ! task from which array should be scattered

   type (distrb), intent(in) :: &
     dst_dist       ! distribution of resulting blocks

   integer (int_kind), dimension(:,:), intent(in) :: &
     ARRAY_G        ! array containing global field on src_task

   integer (int_kind), intent(in) :: &
      field_type,               &! id for type of field (scalar, vector, angle)
      field_loc                  ! id for location on horizontal grid
                                 !  (center, NEcorner, Nface, Eface)

!-----------------------------------------------------------------------
!
!  output variables
!
!-----------------------------------------------------------------------

   integer (int_kind), dimension(:,:,:), intent(inout) :: &
     ARRAY          ! array containing distributed field

!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      i,j,n,bid,         &! dummy loop indices
      isrc, jsrc,        &! source addresses
      xoffset, yoffset,  &! offsets for tripole bounday conditions
      isign,             &! sign factor for tripole boundary conditions
      dst_block           ! local block index in dest distribution

   type (block) :: &
      this_block  ! block info for current block

!-----------------------------------------------------------------------
!
!  initialize return array to zero and set up tripole quantities
!
!-----------------------------------------------------------------------

   ARRAY = c0

   select case (field_loc)
   case (field_loc_center)   ! cell center location
      xoffset = 1
      yoffset = 1
   case (field_loc_NEcorner)   ! cell corner (velocity) location
      xoffset = 0
      yoffset = 0
   case (field_loc_Eface)   ! cell center location
      xoffset = 0
      yoffset = 1
   case (field_loc_Nface)   ! cell corner (velocity) location
      xoffset = 1
      yoffset = 0
   case (field_loc_noupdate) ! ghost cells not needed - use cell center
      xoffset = 1
      yoffset = 1
   end select

   select case (field_type)
   case (field_type_scalar)
      isign =  1
   case (field_type_vector)
      isign = -1
   case (field_type_angle)
      isign = -1
   case (field_type_noupdate) ! ghost cells not needed - use cell center
      isign =  1
   case default
      call exit_POP(sigAbort, 'Unknown field type in scatter')
   end select

!-----------------------------------------------------------------------
!
!  copy blocks of global array into local block distribution
!
!-----------------------------------------------------------------------

   do n=1,nblocks_tot

      if (dst_dist%proc(n) /= 0) then

         this_block = get_block(n,n)
         dst_block  = dst_dist%local_block(n)

         !*** if this is an interior block, then there is no
         !*** padding or update checking required

         if (this_block%iblock > 1         .and. &
             this_block%iblock < nblocks_x .and. &
             this_block%jblock > 1         .and. &
             this_block%jblock < nblocks_y) then

            do j=1,ny_block
            do i=1,nx_block
               ARRAY(i,j,dst_block) = ARRAY_G(this_block%i_glob(i),&
                                              this_block%j_glob(j))
            end do
            end do

         !*** if this is an edge block but not a northern edge
         !*** we only need to check for closed boundaries and
         !*** padding (global index = 0)

         else if (this_block%jblock /= nblocks_y) then

            do j=1,ny_block
               if (this_block%j_glob(j) /= 0) then
                  do i=1,nx_block
                     if (this_block%i_glob(i) /= 0) then
                        ARRAY(i,j,dst_block) = ARRAY_G(this_block%i_glob(i),&
                                                       this_block%j_glob(j))
                     endif
                  end do
               endif
            end do

         !*** if this is a northern edge block, we need to check
         !*** for and properly deal with tripole boundaries

         else

            do j=1,ny_block
               if (this_block%j_glob(j) > 0) then ! normal boundary

                  do i=1,nx_block
                     if (this_block%i_glob(i) /= 0) then
                        ARRAY(i,j,dst_block) = ARRAY_G(this_block%i_glob(i),&
                                                       this_block%j_glob(j))
                     endif
                  end do

               else if (this_block%j_glob(j) < 0) then  ! tripole

                  jsrc = ny_global + yoffset + &
                         (this_block%j_glob(j) + ny_global)
                  do i=1,nx_block
                     if (this_block%i_glob(i) /= 0) then
                        isrc = nx_global + xoffset - this_block%i_glob(i)
                        if (isrc < 1) isrc = isrc + nx_global
                        if (isrc > nx_global) isrc = isrc - nx_global
                        ARRAY(i,j,dst_block) = ARRAY_G(isrc,jsrc)
                     endif
                  end do

               endif
            end do

         endif
      endif ! dst block not land
   end do  ! block loop

!-----------------------------------------------------------------------

 end subroutine scatter_global_int

!EOC
!***********************************************************************
!BOP
! !IROUTINE: redistribute_blocks
! !INTERFACE:

 subroutine redistribute_blocks_dbl(DST_ARRAY, dst_dist, &
                                    SRC_ARRAY, src_dist)

! !DESCRIPTION:
!  This subroutine converts an array distributed in one decomposition
!  to an array distributed in a different decomposition
!
! !REVISION HISTORY:
!  same as module
!
! !REMARKS:
!  This is the specific interface for double precision arrays
!  corresponding to the generic interface scatter_global.  It is shown
!  to provide information on the generic interface (the generic
!  interface is identical, but chooses a specific interface based
!  on the data type of the input argument).

! !INPUT PARAMETERS:

   type (distrb), intent(in) :: &
     src_dist    ,&! info on distribution of blocks for source array
     dst_dist      ! info on distribution of blocks for dest   array

   real (r8), dimension(:,:,:), intent(in) :: &
     SRC_ARRAY     ! array containing field in source distribution

! !OUTPUT PARAMETERS:

   real (r8), dimension(:,:,:), intent(inout) :: &
     DST_ARRAY     ! array containing field in dest distribution

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: n

!-----------------------------------------------------------------------
!
!  copy blocks from one distribution to another
!
!-----------------------------------------------------------------------

   do n=1,nblocks_tot

     if (src_dist%proc(n) /= 0) then

       DST_ARRAY(:,:,dst_dist%local_block(n)) = &
       SRC_ARRAY(:,:,src_dist%local_block(n))

     endif
   end do

!-----------------------------------------------------------------------

 end subroutine redistribute_blocks_dbl

!***********************************************************************

 subroutine redistribute_blocks_real(DST_ARRAY, dst_dist, &
                                     SRC_ARRAY, src_dist)

!-----------------------------------------------------------------------
!
!  This subroutine converts an array distributed in one decomposition
!  to an array distributed in a different decomposition
!
!-----------------------------------------------------------------------
!-----------------------------------------------------------------------
!
!  input variables
!
!-----------------------------------------------------------------------

   type (distrb), intent(in) :: &
     src_dist      ! info on distribution of blocks for source array

   real (r4), dimension(:,:,:), intent(in) :: &
     SRC_ARRAY     ! array containing field in source distribution

!-----------------------------------------------------------------------
!
!  output variables
!
!-----------------------------------------------------------------------

   type (distrb), intent(inout) :: &
     dst_dist      ! info on dist of blocks for destination array

   real (r4), dimension(:,:,:), intent(inout) :: &
     DST_ARRAY     ! array containing field in dest distribution

!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: n

!-----------------------------------------------------------------------
!
!  copy blocks from one distribution to another
!
!-----------------------------------------------------------------------

   do n=1,nblocks_tot

     if (src_dist%proc(n) /= 0) then

       DST_ARRAY(:,:,dst_dist%local_block(n)) = &
       SRC_ARRAY(:,:,src_dist%local_block(n))

     endif
   end do

!-----------------------------------------------------------------------

 end subroutine redistribute_blocks_real

!***********************************************************************

 subroutine redistribute_blocks_int(DST_ARRAY, dst_dist, &
                                    SRC_ARRAY, src_dist)

!-----------------------------------------------------------------------
!
!  This subroutine converts an array distributed in one decomposition
!  to an array distributed in a different decomposition
!
!-----------------------------------------------------------------------
!-----------------------------------------------------------------------
!
!  input variables
!
!-----------------------------------------------------------------------

   type (distrb), intent(in) :: &
     src_dist      ! info on distribution of blocks for source array

   integer (int_kind), dimension(:,:,:), intent(in) :: &
     SRC_ARRAY     ! array containing field in source distribution

!-----------------------------------------------------------------------
!
!  output variables
!
!-----------------------------------------------------------------------

   type (distrb), intent(inout) :: &
     dst_dist      ! info on dist of blocks for destination array

   integer (int_kind), dimension(:,:,:), intent(inout) :: &
     DST_ARRAY     ! array containing field in dest distribution

!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: n

!-----------------------------------------------------------------------
!
!  copy blocks from one distribution to another
!
!-----------------------------------------------------------------------

   do n=1,nblocks_tot

     if (src_dist%proc(n) /= 0) then

       DST_ARRAY(:,:,dst_dist%local_block(n)) = &
       SRC_ARRAY(:,:,src_dist%local_block(n))

     endif
   end do

!-----------------------------------------------------------------------

 end subroutine redistribute_blocks_int

!EOC
!***********************************************************************

 end module gather_scatter

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
