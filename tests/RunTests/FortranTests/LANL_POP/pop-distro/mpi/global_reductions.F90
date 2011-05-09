!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module global_reductions

!BOP
! !MODULE: global_reductions
! !DESCRIPTION:
!  This module contains all the routines for performing global
!  reductions like global sums, minvals, maxvals, etc.
!
! !REVISION HISTORY:
!  CVS:$Id: global_reductions.F90,v 1.5 2003/01/10 00:11:10 jfd Exp $
!  CVS:$Name: POP_2_0_1 $

! !USES:

   use kinds_mod
   use communicate
   use constants
   use blocks
   use distribution
   use domain_size
   use domain
   !use timers

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public :: global_sum,      &
             global_sum_prod, &
             global_count,    &
             global_maxval,   &
             global_minval,   &
             global_maxloc,   &
             init_global_reductions

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  generic interfaces for module procedures
!
!-----------------------------------------------------------------------

   interface global_sum
     module procedure global_sum_dbl,              &
                      global_sum_real,             &
                      global_sum_int,              &
                      global_sum_scalar_dbl,       &
                      global_sum_scalar_real,      &
                      global_sum_scalar_int
   end interface 

   interface global_sum_prod
     module procedure global_sum_prod_dbl,         &
                      global_sum_prod_real,        &
                      global_sum_prod_int
   end interface 

   interface global_count
     module procedure global_count_dbl,            &
                      global_count_real,           &
                      global_count_int,            &
                      global_count_log
   end interface 

   interface global_maxval
     module procedure global_maxval_dbl,           &
                      global_maxval_real,          &
                      global_maxval_int,           &
                      global_maxval_scalar_dbl,    &
                      global_maxval_scalar_real,   &
                      global_maxval_scalar_int
   end interface 

   interface global_minval
     module procedure global_minval_dbl,           &
                      global_minval_real,          &
                      global_minval_int,           &
                      global_minval_scalar_dbl,    &
                      global_minval_scalar_real,   &
                      global_minval_scalar_int
   end interface 

   interface global_maxloc
     module procedure global_maxloc_dbl,           &
                      global_maxloc_real,          &
                      global_maxloc_int
   end interface 

!-----------------------------------------------------------------------
!
!  module variables
!
!-----------------------------------------------------------------------

   !integer (int_kind) :: timer_local, timer_mpi

!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: init_global_reductions
! !INTERFACE:

 subroutine init_global_reductions

! !DESCRIPTION:
!  Initializes necessary buffers for global reductions.
!
! !REVISION HISTORY:
!  same as module
!EOP
!BOC
!-----------------------------------------------------------------------

  !call get_timer(timer_local, 'SUM_LOCAL')
  !call get_timer(timer_mpi  , 'SUM_MPI')

!-----------------------------------------------------------------------
!EOC

 end subroutine init_global_reductions

!***********************************************************************
!BOP
! !IROUTINE: global_sum
! !INTERFACE:

 function global_sum_dbl(X, dist, field_loc, MASK)

! !DESCRIPTION:
!  computes the global sum of the _physical domain_ of a 2-d
!  array.
!
! !REVISION HISTORY:
!  same as module
!
! !REMARKS:
!  This is actually the specific interface for the generic global_sum
!  function corresponding to double precision arrays.  The generic
!  interface is identical but will handle real and integer 2-d slabs
!  and real, integer, and double precision scalars.

! !USES:

   include 'mpif.h'  ! MPI Fortran include file

! !INPUT PARAMETERS:

   real (r8), dimension(:,:,:), intent(in) :: &
      X                    ! array to be summed

   type (distrb), intent(in) :: &
      dist                 ! block distribution for array X

   integer (int_kind), intent(in) :: &
      field_loc            ! location of field on staggered grid

   real (r8), dimension(size(X,dim=1), &
                              size(X,dim=2), &
                              size(X,dim=3)), intent(in), optional :: &
      MASK                 ! real multiplicative mask

! !OUTPUT PARAMETERS:

   real (r8) :: &
      global_sum_dbl       ! resulting global sum

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

!   real (r8), dimension(:), allocatable :: &
!      local_block_sum,    &! sum of local blocks
!      global_block_sum     ! sum of all blocks

   real (r8) ::          &
      local_sum           ! sum of all local blocks

   integer (int_kind) :: &
      i,j,n,             &! local counters
      ib,ie,jb,je,       &! beg,end of physical domain
      bid,               &! block location
      ierr                ! MPI error flag

   type (block) :: &
      this_block          ! holds local block information

!-----------------------------------------------------------------------
!
!  use this code for sums that are not reproducible for best performance
!
!-----------------------------------------------------------------------

   local_sum = c0

   !call timer_start(timer_local)
   if (ltripole_grid .and. (field_loc == field_loc_Nface .or. &
                            field_loc == field_loc_NEcorner)) then
      !*** must remove redundant points from sum
      do n=1,nblocks_tot
         if (dist%proc(n) == my_task+1) then
            bid = dist%local_block(n)
            this_block = get_block(n,bid)
            ib = this_block%ib
            ie = this_block%ie
            jb = this_block%jb
            je = this_block%je
            if (this_block%jblock == nblocks_y) then
               !*** for topmost row in tripole only sum
               !*** 1st half of domain - others are redundant
               if (present(MASK)) then
                  do i=ib,ie
                     if (this_block%i_glob(i) <= nx_global/2) &
                     local_sum = local_sum + X(i,je,bid)*MASK(i,je,bid)
                  end do
               else ! no mask
                  do i=ib,ie
                     if (this_block%i_glob(i) <= nx_global/2) &
                     local_sum = local_sum + X(i,je,bid)
                  end do
               endif
               je = je - 1
            endif
            if (present(MASK)) then
               do j=jb,je
               do i=ib,ie
                  local_sum = local_sum + X(i,j,bid)*MASK(i,j,bid)
               end do
               end do
            else ! no mask
               do j=jb,je
               do i=ib,ie
                  local_sum = local_sum + X(i,j,bid)
               end do
               end do
            endif
         endif
      end do !block loop
   else ! regular global sum
      do n=1,nblocks_tot
         if (dist%proc(n) == my_task+1) then
            bid = dist%local_block(n)
            call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
            if (present(MASK)) then
               do j=jb,je
               do i=ib,ie
                  local_sum = local_sum + X(i,j,bid)*MASK(i,j,bid)
               end do
               end do
            else ! no mask
               do j=jb,je
               do i=ib,ie
                  local_sum = local_sum + X(i,j,bid)
               end do
               end do
            endif
         endif
      end do !block loop
   endif
   !call timer_stop(timer_local)

   !call timer_start(timer_mpi)
   if (dist%nprocs > 1) then
      if (my_task < dist%nprocs) then
         call MPI_ALLREDUCE(local_sum, global_sum_dbl, 1, &
                            mpi_dbl, MPI_SUM, dist%communicator, ierr)
      else
         global_sum_dbl = c0
      endif
   else
      global_sum_dbl = local_sum
   endif
   !call timer_stop(timer_mpi)

!-----------------------------------------------------------------------
!
!  use this code for sums that are more reproducible - performance
!  of this code does not scale well for large numbers of blocks
!  NOTE: THIS CODE NOT MODIFIED FOR TRIPOLE GRIDS YET
!
!-----------------------------------------------------------------------
!
!   allocate (local_block_sum(nblocks_tot), &
!             global_block_sum(nblocks_tot))
!
!   local_block_sum = c0
!
!   !call timer_start(timer_local)
!   if (present(MASK)) then
!     do n=1,nblocks_tot
!       if (dist%proc(n) == my_task+1) then
!         bid = dist%local_block(n)
!         call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
!         do j=jb,je
!         do i=ib,ie
!           local_block_sum(n) = &
!           local_block_sum(n) + X(i,j,bid)*MASK(i,j,bid)
!         end do
!         end do
!       endif
!     end do
!   else
!     do n=1,nblocks_tot
!       if (dist%proc(n) == my_task+1) then
!         bid = dist%local_block(n)
!         call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
!         do j=jb,je
!         do i=ib,ie
!           local_block_sum(n) = &
!           local_block_sum(n) + X(i,j,bid)
!         end do
!         end do
!       endif
!     end do
!   endif
!   !call timer_stop(timer_local)
!
!   !call timer_start(timer_mpi)
!   call MPI_ALLREDUCE(local_block_sum, global_block_sum, nblocks_tot, &
!                      mpi_dbl, MPI_SUM, dist%communicator, ierr)
!   !call timer_stop(timer_mpi)
!
!   global_sum_dbl = c0
!   do n=1,nblocks_tot
!     global_sum_dbl = global_sum_dbl + global_block_sum(n)
!   end do
!
!   deallocate ( local_block_sum, global_block_sum)
!
!-----------------------------------------------------------------------

 end function global_sum_dbl

!***********************************************************************

 function global_sum_real(X, dist, field_loc, MASK)

!-----------------------------------------------------------------------
!
!  computes the global sum of the _physical domain_ of a 2-d
!  array.
!
!-----------------------------------------------------------------------

   include 'mpif.h'  ! MPI Fortran include file

!-----------------------------------------------------------------------
!
!  input vars
!
!-----------------------------------------------------------------------

   real (r4), dimension(:,:,:), intent(in) :: &
      X                    ! array to be summed

   type (distrb), intent(in) :: &
      dist                 ! block distribution for array X

   integer (int_kind), intent(in) :: &
      field_loc            ! location of field on staggered grid

   real (r8), dimension(size(X,dim=1), &
                              size(X,dim=2), &
                              size(X,dim=3)), intent(in), optional :: &
      MASK                 ! real multiplicative mask

!-----------------------------------------------------------------------
!
!  output result
!
!-----------------------------------------------------------------------

   real (r4) :: &
      global_sum_real       ! resulting global sum

!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

!   real (r4), dimension(:), allocatable :: &
!      local_block_sum,    &! sum of local blocks
!      global_block_sum     ! sum of all blocks

   real (r4) ::          &
      local_sum           ! sum of local blocks

   integer (int_kind) :: &
      i,j,n,             &! local counters
      ib,ie,jb,je,       &! beg,end of physical domain
      bid,               &! block location
      ierr                ! MPI error flag

   type (block) :: &
      this_block          ! holds local block information

!-----------------------------------------------------------------------
!
!  use this code for sums that perform better but are not reproducible
!
!-----------------------------------------------------------------------

   local_sum = c0

   !call timer_start(timer_local)
   if (ltripole_grid .and. (field_loc == field_loc_Nface .or. &
                            field_loc == field_loc_NEcorner)) then
      !*** must remove redundant points from sum
      do n=1,nblocks_tot
         if (dist%proc(n) == my_task+1) then
            bid = dist%local_block(n)
            this_block = get_block(n,bid)
            ib = this_block%ib
            ie = this_block%ie
            jb = this_block%jb
            je = this_block%je
            if (this_block%jblock == nblocks_y) then
               !*** for topmost row in tripole only sum
               !*** 1st half of domain - others are redundant
               if (present(MASK)) then
                  do i=ib,ie
                     if (this_block%i_glob(i) <= nx_global/2) &
                     local_sum = local_sum + X(i,je,bid)*MASK(i,je,bid)
                  end do
               else ! no mask
                  do i=ib,ie
                     if (this_block%i_glob(i) <= nx_global/2) &
                     local_sum = local_sum + X(i,je,bid)
                  end do
               endif
               je = je - 1
            endif
            if (present(MASK)) then
               do j=jb,je
               do i=ib,ie
                  local_sum = local_sum + X(i,j,bid)*MASK(i,j,bid)
               end do
               end do
            else ! no mask
               do j=jb,je
               do i=ib,ie
                  local_sum = local_sum + X(i,j,bid)
               end do
               end do
            endif
         endif
      end do !block loop
   else ! regular global sum
      do n=1,nblocks_tot
         if (dist%proc(n) == my_task+1) then
            bid = dist%local_block(n)
            call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
            if (present(MASK)) then
               do j=jb,je
               do i=ib,ie
                  local_sum = local_sum + X(i,j,bid)*MASK(i,j,bid)
               end do
               end do
            else ! no mask
               do j=jb,je
               do i=ib,ie
                  local_sum = local_sum + X(i,j,bid)
               end do
               end do
            endif
         endif
      end do !block loop
   endif
   !call timer_stop(timer_local)

   if (dist%nprocs > 1) then
      if (my_task < dist%nprocs) then
         call MPI_ALLREDUCE(local_sum, global_sum_real, 1, &
                            mpi_real, MPI_SUM, dist%communicator, ierr)
      else
         global_sum_real = c0
      endif
   else
      global_sum_real = local_sum
   endif

!-----------------------------------------------------------------------
!
!  use this code for sums that are more reproducible - performance
!  of this code does not scale well for large numbers of blocks
!  NOTE: THIS CODE NOT MODIFIED FOR TRIPOLE GRIDS YET
!
!-----------------------------------------------------------------------
!
!   allocate (local_block_sum(nblocks_tot), &
!             global_block_sum(nblocks_tot))
!
!   local_block_sum = c0
!
!   if (present(MASK)) then
!     do n=1,nblocks_tot
!       if (dist%proc(n) == my_task+1) then
!         bid = dist%local_block(n)
!         call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
!         do j=jb,je
!         do i=ib,ie
!           local_block_sum(n) = &
!           local_block_sum(n) + X(i,j,bid)*MASK(i,j,bid)
!         end do
!         end do
!       endif
!     end do
!   else
!     do n=1,nblocks_tot
!       if (dist%proc(n) == my_task+1) then
!         bid = dist%local_block(n)
!         call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
!         do j=jb,je
!         do i=ib,ie
!           local_block_sum(n) = &
!           local_block_sum(n) + X(i,j,bid)
!         end do
!         end do
!       endif
!     end do
!   endif
!
!   call MPI_ALLREDUCE(local_block_sum, global_block_sum, nblocks_tot, &
!                      mpi_real, MPI_SUM, dist%communicator, ierr)
!
!   global_sum_real = c0
!   do n=1,nblocks_tot
!     global_sum_real = global_sum_real + global_block_sum(n)
!   end do
!
!   deallocate ( local_block_sum, global_block_sum)
!
!-----------------------------------------------------------------------

 end function global_sum_real

!***********************************************************************

 function global_sum_int(X, dist, field_loc, MASK)

!-----------------------------------------------------------------------
!
!  computes the global sum of the _physical domain_ of a 2-d
!  array.
!
!-----------------------------------------------------------------------

   include 'mpif.h'  ! MPI Fortran include file

!-----------------------------------------------------------------------
!
!  input vars
!
!-----------------------------------------------------------------------

   integer (int_kind), dimension(:,:,:), intent(in) :: &
      X                    ! array to be summed

   type (distrb), intent(in) :: &
      dist                 ! block distribution for array X

   integer (int_kind), intent(in) :: &
      field_loc            ! location of field on staggered grid

   real (r8), dimension(size(X,dim=1), &
                              size(X,dim=2), &
                              size(X,dim=3)), intent(in), optional :: &
      MASK                 ! real multiplicative mask

!-----------------------------------------------------------------------
!
!  output result
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      global_sum_int       ! resulting global sum

!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

!   integer (int_kind), dimension(:), allocatable :: &
!      local_block_sum,    &! sum of local blocks
!      global_block_sum     ! sum of all blocks

   integer (int_kind) :: &
      local_sum           ! sum of local blocks

   integer (int_kind) :: &
      i,j,n,             &! local counters
      ib,ie,jb,je,       &! beg,end of physical domain
      bid,               &! block location
      ierr                ! MPI error flag

   type (block) :: &
      this_block          ! holds local block information

!-----------------------------------------------------------------------
!
!  use this code for sums that are not reproducible but perform better
!
!-----------------------------------------------------------------------

   local_sum = c0

   !call timer_start(timer_local)
   if (ltripole_grid .and. (field_loc == field_loc_Nface .or. &
                            field_loc == field_loc_NEcorner)) then
      !*** must remove redundant points from sum
      do n=1,nblocks_tot
         if (dist%proc(n) == my_task+1) then
            bid = dist%local_block(n)
            this_block = get_block(n,bid)
            ib = this_block%ib
            ie = this_block%ie
            jb = this_block%jb
            je = this_block%je
            if (this_block%jblock == nblocks_y) then
               !*** for topmost row in tripole only sum
               !*** 1st half of domain - others are redundant
               if (present(MASK)) then
                  do i=ib,ie
                     if (this_block%i_glob(i) <= nx_global/2) &
                     local_sum = local_sum + X(i,je,bid)*MASK(i,je,bid)
                  end do
               else ! no mask
                  do i=ib,ie
                     if (this_block%i_glob(i) <= nx_global/2) &
                     local_sum = local_sum + X(i,je,bid)
                  end do
               endif
               je = je - 1
            endif
            if (present(MASK)) then
               do j=jb,je
               do i=ib,ie
                  local_sum = local_sum + X(i,j,bid)*MASK(i,j,bid)
               end do
               end do
            else ! no mask
               do j=jb,je
               do i=ib,ie
                  local_sum = local_sum + X(i,j,bid)
               end do
               end do
            endif
         endif
      end do !block loop
   else ! regular global sum
      do n=1,nblocks_tot
         if (dist%proc(n) == my_task+1) then
            bid = dist%local_block(n)
            call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
            if (present(MASK)) then
               do j=jb,je
               do i=ib,ie
                  local_sum = local_sum + X(i,j,bid)*MASK(i,j,bid)
               end do
               end do
            else ! no mask
               do j=jb,je
               do i=ib,ie
                  local_sum = local_sum + X(i,j,bid)
               end do
               end do
            endif
         endif
      end do !block loop
   endif
   !call timer_stop(timer_local)

   if (dist%nprocs > 1) then
      if (my_task < dist%nprocs) then
         call MPI_ALLREDUCE(local_sum, global_sum_int, 1, &
                       mpi_integer, MPI_SUM, dist%communicator, ierr)
      else
         global_sum_int = 0
      endif
   else
      global_sum_int = local_sum
   endif

!-----------------------------------------------------------------------
!
!  use this code for sums that are more reproducible - performance
!  of this code does not scale well for large numbers of blocks
!  NOTE: THIS CODE NOT YET CHANGED FOR TRIPOLE GRID
!
!-----------------------------------------------------------------------
!
!   allocate (local_block_sum(nblocks_tot), &
!             global_block_sum(nblocks_tot))
!
!   local_block_sum = c0
!
!   if (present(MASK)) then
!     do n=1,nblocks_tot
!       if (dist%proc(n) == my_task+1) then
!         bid = dist%local_block(n)
!         call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
!         do j=jb,je
!         do i=ib,ie
!           local_block_sum(n) = &
!           local_block_sum(n) + X(i,j,bid)*MASK(i,j,bid)
!         end do
!         end do
!       endif
!     end do
!   else
!     do n=1,nblocks_tot
!       if (dist%proc(n) == my_task+1) then
!         bid = dist%local_block(n)
!         call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
!         do j=jb,je
!         do i=ib,ie
!           local_block_sum(n) = &
!           local_block_sum(n) + X(i,j,bid)
!         end do
!         end do
!       endif
!     end do
!   endif
!
!   call MPI_ALLREDUCE(local_block_sum, global_block_sum, nblocks_tot, &
!                      mpi_integer, MPI_SUM, dist%communicator, ierr)
!
!   global_sum_int = 0
!   do n=1,nblocks_tot
!     global_sum_int = global_sum_int + global_block_sum(n)
!   end do
!
!   deallocate ( local_block_sum, global_block_sum)
!
!-----------------------------------------------------------------------

 end function global_sum_int

!***********************************************************************

 function global_sum_scalar_dbl(local_scalar, dist)

!-----------------------------------------------------------------------
!
!  this function returns the sum of scalar value across processors
!
!-----------------------------------------------------------------------

   include 'mpif.h'  ! MPI Fortran include file

   type (distrb), intent(in) :: &
      dist                 ! distribution from which this is called

   real (r8), intent(inout) :: &
      local_scalar                ! local scalar to be compared

   real (r8) :: &
      global_sum_scalar_dbl   ! resulting global sum

   integer (int_kind) :: ierr ! MPI error flag

!-----------------------------------------------------------------------

   if (dist%nprocs > 1) then
      if (my_task < dist%nprocs) then
         call MPI_ALLREDUCE(local_scalar, global_sum_scalar_dbl, 1, &
                            mpi_dbl, MPI_SUM, dist%communicator, ierr)
      else
         global_sum_scalar_dbl = c0
      endif
   else
      global_sum_scalar_dbl = local_scalar
   endif

!-----------------------------------------------------------------------

 end function global_sum_scalar_dbl

!***********************************************************************

 function global_sum_scalar_real(local_scalar, dist)

!-----------------------------------------------------------------------
!
!  this function returns the sum of scalar value across processors
!
!-----------------------------------------------------------------------

   include 'mpif.h'  ! MPI Fortran include file

   real (r4), intent(inout) :: &
      local_scalar                ! local scalar to be compared

   type (distrb), intent(in) :: &
      dist                 ! distribution from which this is called

   real (r4) :: &
      global_sum_scalar_real   ! resulting global sum

   integer (int_kind) :: ierr ! MPI error flag

!-----------------------------------------------------------------------

   if (dist%nprocs > 1) then
      if (my_task < dist%nprocs) then
         call MPI_ALLREDUCE(local_scalar, global_sum_scalar_real, 1, &
                            MPI_REAL, MPI_SUM, dist%communicator, ierr)
      else
         global_sum_scalar_real = c0
      endif
   else
      global_sum_scalar_real = local_scalar
   endif

!-----------------------------------------------------------------------

 end function global_sum_scalar_real

!***********************************************************************

 function global_sum_scalar_int(local_scalar, dist)

!-----------------------------------------------------------------------
!
!  this function returns the sum of scalar value across processors
!
!-----------------------------------------------------------------------

   include 'mpif.h'  ! MPI Fortran include file

   integer (int_kind), intent(inout) :: &
      local_scalar                ! local scalar to be compared

   type (distrb), intent(in) :: &
      dist                 ! distribution from which this is called

   integer (int_kind) :: &
      global_sum_scalar_int   ! resulting global sum

   integer (int_kind) :: ierr ! MPI error flag

!-----------------------------------------------------------------------

   if (dist%nprocs > 1) then
      if (my_task < dist%nprocs) then
         call MPI_ALLREDUCE(local_scalar, global_sum_scalar_int, 1, &
                         MPI_INTEGER, MPI_SUM, dist%communicator, ierr)
      else
         global_sum_scalar_int = 0
      endif
   else
      global_sum_scalar_int = local_scalar
   endif

!-----------------------------------------------------------------------

 end function global_sum_scalar_int

!EOC
!***********************************************************************
!BOP
! !IROUTINE: global_sum_prod
! !INTERFACE:

 function global_sum_prod_dbl (X,Y,dist,field_loc, MASK)

! !DESCRIPTION:
!  this routine performs a global sum over the physical domain
!  of a product of two 2-d arrays.
!
! !REVISION HISTORY:
!  same as module
!
! !REMARKS:
!  This is actually the specific interface for the generic 
!  global_sum_prod function corresponding to double precision arrays.
!  The generic interface is identical but will handle real and integer 
!  2-d slabs.

! !USES:

   include 'mpif.h'  ! MPI Fortran include file

! !INPUT PARAMETERS:

   real (r8), dimension(:,:,:), intent(in) :: &
     X,                &! first array in product to be summed
     Y                  ! second array in product to be summed

   type (distrb), intent(in) :: &
      dist                 ! block distribution for array X,Y

   integer (int_kind), intent(in) :: &
      field_loc            ! location of field on staggered grid

   real (r8), &
     dimension(size(X,dim=1),size(X,dim=2),size(X,dim=3)), &
     intent(in), optional :: &
     MASK               ! real multiplicative mask

! !OUTPUT PARAMETERS:

   real (r8) :: &
     global_sum_prod_dbl ! resulting global sum of X*Y

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

!   real (r8), dimension(:), allocatable :: &
!     local_block_sum,  &! sum of each block
!     global_block_sum   ! global sum each block

   real (r8) ::         & 
     local_sum           ! sum of each block

   integer (int_kind) :: &
      i,j,n,            &! dummy counters
      bid,              &! local block location
      ib,ie,jb,je,      &! beg,end indices of physical domain
      ierr               ! MPI error flag

   type (block) :: &
      this_block          ! holds local block information

!-----------------------------------------------------------------------
!
!  use this code for sums that are not reproducible but perform better
!
!-----------------------------------------------------------------------

   local_sum = c0

   !call timer_start(timer_local)
   if (ltripole_grid .and. (field_loc == field_loc_Nface .or. &
                            field_loc == field_loc_NEcorner)) then
      !*** must remove redundant points from sum
      do n=1,nblocks_tot
         if (dist%proc(n) == my_task+1) then
            bid = dist%local_block(n)
            this_block = get_block(n,bid)
            ib = this_block%ib
            ie = this_block%ie
            jb = this_block%jb
            je = this_block%je
            if (this_block%jblock == nblocks_y) then
               !*** for topmost row in tripole only sum
               !*** 1st half of domain - others are redundant
               if (present(MASK)) then
                  do i=ib,ie
                     if (this_block%i_glob(i) <= nx_global/2) &
                     local_sum = local_sum + &
                                 X(i,je,bid)*Y(i,je,bid)*MASK(i,je,bid)
                  end do
               else ! no mask
                  do i=ib,ie
                     if (this_block%i_glob(i) <= nx_global/2) &
                     local_sum = local_sum + X(i,je,bid)*Y(i,je,bid)
                  end do
               endif
               je = je - 1
            endif
            if (present(MASK)) then
               do j=jb,je
               do i=ib,ie
                  local_sum = local_sum + &
                              X(i,j,bid)*Y(i,j,bid)*MASK(i,j,bid)
               end do
               end do
            else ! no mask
               do j=jb,je
               do i=ib,ie
                  local_sum = local_sum + X(i,j,bid)*Y(i,j,bid)
               end do
               end do
            endif
         endif
      end do !block loop
   else ! regular global sum
      do n=1,nblocks_tot
         if (dist%proc(n) == my_task+1) then
            bid = dist%local_block(n)
            call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
            if (present(MASK)) then
               do j=jb,je
               do i=ib,ie
                  local_sum = local_sum + &
                              X(i,j,bid)*Y(i,j,bid)*MASK(i,j,bid)
               end do
               end do
            else ! no mask
               do j=jb,je
               do i=ib,ie
                  local_sum = local_sum + X(i,j,bid)*Y(i,j,bid)
               end do
               end do
            endif
         endif
      end do !block loop
   endif
   !call timer_stop(timer_local)

   if (dist%nprocs > 1) then
      if (my_task < dist%nprocs) then
         call MPI_ALLREDUCE(local_sum, global_sum_prod_dbl, 1, &
                      mpi_dbl, MPI_SUM, dist%communicator, ierr)
      else
         global_sum_prod_dbl = 0
      endif
   else
      global_sum_prod_dbl = local_sum
   endif

!-----------------------------------------------------------------------
!
!  use this code for sums that are more reproducible - performance
!  of this code does not scale well for large numbers of blocks
!  NOTE: THIS CODE NOT YET CHANGED FOR TRIPOLE GRIDS
!
!-----------------------------------------------------------------------
!
!   allocate (local_block_sum(nblocks_tot), &
!             global_block_sum(nblocks_tot))
!
!   local_block_sum = c0
!
!   if (present(MASK)) then
!     do n=1,nblocks_tot
!       if (dist%proc(n) == my_task+1) then
!         bid = dist%local_block(n)
!         call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
!         do j=jb,je
!         do i=ib,ie
!           local_block_sum(n) = &
!           local_block_sum(n) + X(i,j,bid)*Y(i,j,bid)*MASK(i,j,bid)
!         end do
!         end do
!       endif
!     end do
!   else
!     do n=1,nblocks_tot
!       if (dist%proc(n) == my_task+1) then
!         bid = dist%local_block(n)
!         call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
!         do j=jb,je
!         do i=ib,ie
!           local_block_sum(n) = &
!           local_block_sum(n) + X(i,j,bid)*Y(i,j,bid)
!         end do
!         end do
!       endif
!     end do
!   endif
!
!   call MPI_ALLREDUCE(local_block_sum, global_block_sum, nblocks_tot, &
!                      mpi_dbl, MPI_SUM, dist%communicator, ierr)
!
!   global_sum_prod_dbl = c0
!   do n=1,nblocks_tot
!     global_sum_prod_dbl = global_sum_prod_dbl + global_block_sum(n)
!   end do
!
!   deallocate ( local_block_sum, global_block_sum)
!
!-----------------------------------------------------------------------

 end function global_sum_prod_dbl

!***********************************************************************

 function global_sum_prod_real (X, Y, dist, field_loc, MASK)

!-----------------------------------------------------------------------
!
!  this routine performs a global sum over the physical domain
!  of a product of two 2-d arrays.
!
!-----------------------------------------------------------------------

   include 'mpif.h'  ! MPI Fortran include file

!-----------------------------------------------------------------------
!
!  input variables
!
!-----------------------------------------------------------------------

   real (r4), dimension(:,:,:), intent(in) :: &
     X,                &! first array in product to be summed
     Y                  ! second array in product to be summed

   type (distrb), intent(in) :: &
      dist                 ! block distribution for array X,Y

   integer (int_kind), intent(in) :: &
      field_loc            ! location of field on staggered grid

   real (r8), &
     dimension(size(X,dim=1),size(X,dim=2),size(X,dim=3)), &
     intent(in), optional :: &
     MASK               ! real multiplicative mask

!-----------------------------------------------------------------------
!
!  output variables
!
!-----------------------------------------------------------------------

   real (r4) :: &
     global_sum_prod_real ! resulting global sum of X*Y

!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

!   real (r8), dimension(:), allocatable :: &
!     local_block_sum,  &! sum of each block
!     global_block_sum   ! global sum each block

   real (r8) ::         &
     local_sum,         &! sum of local blocks
     global_sum_prod_tmp ! sum of global blocks

   integer (int_kind) :: &
      i,j,n,            &! dummy counters
      bid,              &! local block location
      ib,ie,jb,je,      &! beg,end indices of physical domain
      ierr               ! MPI error flag

   type (block) :: &
      this_block          ! holds local block information

!-----------------------------------------------------------------------
!
!  use this code for sums that are not reproducible but perform better
!
!-----------------------------------------------------------------------

   local_sum = c0

   !call timer_start(timer_local)
   if (ltripole_grid .and. (field_loc == field_loc_Nface .or. &
                            field_loc == field_loc_NEcorner)) then
      !*** must remove redundant points from sum
      do n=1,nblocks_tot
         if (dist%proc(n) == my_task+1) then
            bid = dist%local_block(n)
            this_block = get_block(n,bid)
            ib = this_block%ib
            ie = this_block%ie
            jb = this_block%jb
            je = this_block%je
            if (this_block%jblock == nblocks_y) then
               !*** for topmost row in tripole only sum
               !*** 1st half of domain - others are redundant
               if (present(MASK)) then
                  do i=ib,ie
                     if (this_block%i_glob(i) <= nx_global/2) &
                     local_sum = local_sum + &
                                 X(i,je,bid)*Y(i,je,bid)*MASK(i,je,bid)
                  end do
               else ! no mask
                  do i=ib,ie
                     if (this_block%i_glob(i) <= nx_global/2) &
                     local_sum = local_sum + X(i,je,bid)*Y(i,je,bid)
                  end do
               endif
               je = je - 1
            endif
            if (present(MASK)) then
               do j=jb,je
               do i=ib,ie
                  local_sum = local_sum + &
                              X(i,j,bid)*Y(i,j,bid)*MASK(i,j,bid)
               end do
               end do
            else ! no mask
               do j=jb,je
               do i=ib,ie
                  local_sum = local_sum + X(i,j,bid)*Y(i,j,bid)
               end do
               end do
            endif
         endif
      end do !block loop
   else ! regular global sum
      do n=1,nblocks_tot
         if (dist%proc(n) == my_task+1) then
            bid = dist%local_block(n)
            call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
            if (present(MASK)) then
               do j=jb,je
               do i=ib,ie
                  local_sum = local_sum + &
                              X(i,j,bid)*Y(i,j,bid)*MASK(i,j,bid)
               end do
               end do
            else ! no mask
               do j=jb,je
               do i=ib,ie
                  local_sum = local_sum + X(i,j,bid)*Y(i,j,bid)
               end do
               end do
            endif
         endif
      end do !block loop
   endif
   !call timer_stop(timer_local)

   if (dist%nprocs > 1) then
      if (my_task < dist%nprocs) then
         call MPI_ALLREDUCE(local_sum, global_sum_prod_tmp, 1, &
                      mpi_dbl, MPI_SUM, dist%communicator, ierr)
         global_sum_prod_real = global_sum_prod_tmp
      else
         global_sum_prod_real = 0
      endif
   else
      global_sum_prod_real = local_sum
   endif

!-----------------------------------------------------------------------
!
!  use this code for sums that are more reproducible - performance
!  of this code does not scale well for large numbers of blocks
!  NOTE: THIS CODE NOT YET CHANGED FOR TRIPOLE GRIDS
!
!-----------------------------------------------------------------------
!
!
!   allocate (local_block_sum(nblocks_tot), &
!             global_block_sum(nblocks_tot))
!
!   local_block_sum = c0
!
!   if (present(MASK)) then
!     do n=1,nblocks_tot
!       if (dist%proc(n) == my_task+1) then
!         bid = dist%local_block(n)
!         call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
!         do j=jb,je
!         do i=ib,ie
!           local_block_sum(n) = &
!           local_block_sum(n) + X(i,j,bid)*Y(i,j,bid)*MASK(i,j,bid)
!         end do
!         end do
!       endif
!     end do
!   else
!     do n=1,nblocks_tot
!       if (dist%proc(n) == my_task+1) then
!         bid = dist%local_block(n)
!         call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
!         do j=jb,je
!         do i=ib,ie
!           local_block_sum(n) = &
!           local_block_sum(n) + X(i,j,bid)*Y(i,j,bid)
!         end do
!         end do
!       endif
!     end do
!   endif
!
!   call MPI_ALLREDUCE(local_block_sum, global_block_sum, nblocks_tot, &
!                      mpi_dbl, MPI_SUM, dist%communicator, ierr)
!
!   global_sum_prod_real = c0
!   do n=1,nblocks_tot
!     global_sum_prod_real = global_sum_prod_real + global_block_sum(n)
!   end do
!
!   deallocate ( local_block_sum, global_block_sum)
!
!-----------------------------------------------------------------------

 end function global_sum_prod_real

!***********************************************************************

 function global_sum_prod_int (X, Y, dist, field_loc, MASK)

!-----------------------------------------------------------------------
!
!  this routine performs a global sum over the physical domain
!  of a product of two 2-d arrays.
!
!-----------------------------------------------------------------------

   include 'mpif.h'  ! MPI Fortran include file

!-----------------------------------------------------------------------
!
!  input variables
!
!-----------------------------------------------------------------------

   integer (int_kind), dimension(:,:,:), intent(in) :: &
     X,                &! first array in product to be summed
     Y                  ! second array in product to be summed

   type (distrb), intent(in) :: &
      dist                 ! block distribution for array X,Y

   integer (int_kind), intent(in) :: &
      field_loc            ! location of field on staggered grid

   real (r8), &
     dimension(size(X,dim=1),size(X,dim=2),size(X,dim=3)), &
     intent(in), optional :: &
     MASK               ! real multiplicative mask

!-----------------------------------------------------------------------
!
!  output variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
     global_sum_prod_int ! resulting global sum of X*Y

!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

!   integer (int_kind), dimension(:), allocatable :: &
!     local_block_sum,  &! sum of each block
!     global_block_sum   ! global sum each block

   integer (int_kind) :: &
     local_sum           ! sum of local blocks

   integer (int_kind) :: &
      i,j,n,            &! dummy counters
      bid,              &! local block location
      ib,ie,jb,je,      &! beg,end indices of physical domain
      ierr               ! MPI error flag

   type (block) :: &
      this_block          ! holds local block information

!-----------------------------------------------------------------------
!
!  use this code for sums that are not reproducible but perform better
!
!-----------------------------------------------------------------------

   local_sum = c0

   !call timer_start(timer_local)
   if (ltripole_grid .and. (field_loc == field_loc_Nface .or. &
                            field_loc == field_loc_NEcorner)) then
      !*** must remove redundant points from sum
      do n=1,nblocks_tot
         if (dist%proc(n) == my_task+1) then
            bid = dist%local_block(n)
            this_block = get_block(n,bid)
            ib = this_block%ib
            ie = this_block%ie
            jb = this_block%jb
            je = this_block%je
            if (this_block%jblock == nblocks_y) then
               !*** for topmost row in tripole only sum
               !*** 1st half of domain - others are redundant
               if (present(MASK)) then
                  do i=ib,ie
                     if (this_block%i_glob(i) <= nx_global/2) &
                     local_sum = local_sum + &
                                 X(i,je,bid)*Y(i,je,bid)*MASK(i,je,bid)
                  end do
               else ! no mask
                  do i=ib,ie
                     if (this_block%i_glob(i) <= nx_global/2) &
                     local_sum = local_sum + X(i,je,bid)*Y(i,je,bid)
                  end do
               endif
               je = je - 1
            endif
            if (present(MASK)) then
               do j=jb,je
               do i=ib,ie
                  local_sum = local_sum + &
                              X(i,j,bid)*Y(i,j,bid)*MASK(i,j,bid)
               end do
               end do
            else ! no mask
               do j=jb,je
               do i=ib,ie
                  local_sum = local_sum + X(i,j,bid)*Y(i,j,bid)
               end do
               end do
            endif
         endif
      end do !block loop
   else ! regular global sum
      do n=1,nblocks_tot
         if (dist%proc(n) == my_task+1) then
            bid = dist%local_block(n)
            call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
            if (present(MASK)) then
               do j=jb,je
               do i=ib,ie
                  local_sum = local_sum + &
                              X(i,j,bid)*Y(i,j,bid)*MASK(i,j,bid)
               end do
               end do
            else ! no mask
               do j=jb,je
               do i=ib,ie
                  local_sum = local_sum + X(i,j,bid)*Y(i,j,bid)
               end do
               end do
            endif
         endif
      end do !block loop
   endif
   !call timer_stop(timer_local)

   if (dist%nprocs > 1) then
      if (my_task < dist%nprocs) then
         call MPI_ALLREDUCE(local_sum, global_sum_prod_int, 1, &
                      mpi_integer, MPI_SUM, dist%communicator, ierr)
      else
         global_sum_prod_int = 0
      endif
   else
      global_sum_prod_int = local_sum
   endif

!-----------------------------------------------------------------------
!
!  use this code for sums that are more reproducible - performance
!  of this code does not scale well for large numbers of blocks
!  NOTE: THIS CODE NOT YET CHANGED FOR TRIPOLE GRIDS
!
!-----------------------------------------------------------------------
!
!   allocate (local_block_sum(nblocks_tot), &
!             global_block_sum(nblocks_tot))
!
!   local_block_sum = c0
!
!   if (present(MASK)) then
!     do n=1,nblocks_tot
!       if (dist%proc(n) == my_task+1) then
!         bid = dist%local_block(n)
!         call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
!         do j=jb,je
!         do i=ib,ie
!           local_block_sum(n) = &
!           local_block_sum(n) + X(i,j,bid)*Y(i,j,bid)*MASK(i,j,bid)
!         end do
!         end do
!       endif
!     end do
!   else
!     do n=1,nblocks_tot
!       if (dist%proc(n) == my_task+1) then
!         bid = dist%local_block(n)
!         call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
!         do j=jb,je
!         do i=ib,ie
!           local_block_sum(n) = &
!           local_block_sum(n) + X(i,j,bid)*Y(i,j,bid)
!         end do
!         end do
!       endif
!     end do
!   endif
!
!   call MPI_ALLREDUCE(local_block_sum, global_block_sum, nblocks_tot, &
!                      mpi_integer, MPI_SUM, dist%communicator, ierr)
!
!   global_sum_prod_int = 0
!   do n=1,nblocks_tot
!     global_sum_prod_int = global_sum_prod_int + global_block_sum(n)
!   end do
!
!   deallocate ( local_block_sum, global_block_sum)
!
!-----------------------------------------------------------------------

 end function global_sum_prod_int

!EOC
!***********************************************************************
!BOP
! !IROUTINE: global_count
! !INTERFACE:

 function global_count_dbl (MASK, dist, field_loc)

! !DESCRIPTION:
!  This function returns the number of true or non-zero elements
!  in the physical domain of a 2-d array.
!
! !REVISION HISTORY:
!  same as module
!
! !REMARKS:
!  This is actually the specific interface for the generic 
!  global_count function corresponding to double precision arrays.
!  The generic interface is identical but will handle real and integer 
!  and logical arrays.

! !USES:

   include 'mpif.h'  ! MPI Fortran include file

!INPUT PARAMETERS:

   real (r8), dimension(:,:,:), intent(in) :: &
      MASK

   type (distrb), intent(in) :: &
      dist                 ! block distribution for MASK

   integer (int_kind), intent(in) :: &
      field_loc            ! location of field on staggered grid

!OUTPUT PARAMETERS:

   integer (int_kind) :: &
      global_count_dbl      ! resulting global count

!EOP
!BOC

   integer (int_kind) :: &
      local_count,         &! local count of subdomain
      ierr,                &! MPI error flag
      ib, ie, jb, je,      &! start,end of physical domain
      bid,                 &! block location in distribution
      i, n                  ! dummy counters

   type (block) :: &
      this_block          ! holds local block information

!-----------------------------------------------------------------------

   local_count = 0
   if (ltripole_grid .and. (field_loc == field_loc_Nface .or. &
                            field_loc == field_loc_NEcorner)) then
      !*** must remove redundant points from sum
      do n=1,nblocks_tot
         if (dist%proc(n) == my_task+1) then
            bid = dist%local_block(n)
            this_block = get_block(n,bid)
            ib = this_block%ib
            ie = this_block%ie
            jb = this_block%jb
            je = this_block%je
            if (this_block%jblock == nblocks_y) then
               !*** for topmost row in tripole only sum
               !*** 1st half of domain - others are redundant
               do i=ib,ie
                  if (this_block%i_glob(i) <= nx_global/2 .and. &
                      MASK(i,je,bid) /= c0) &
                     local_count = local_count + 1
               end do
               je = je - 1
            endif
            local_count = local_count &
                        + count(MASK(ib:ie,jb:je,bid) /= c0)
         endif
      end do !block loop
   else ! regular global count
      do n=1,nblocks_tot
         if (dist%proc(n) == my_task+1) then
            bid = dist%local_block(n)
            call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
            local_count = local_count &
                        + count(MASK(ib:ie,jb:je,bid) /= c0)
         endif
      end do !block loop
   endif

   if (dist%nprocs > 1) then
      if (my_task < dist%nprocs) then
         call MPI_ALLREDUCE(local_count, global_count_dbl, 1, &
                         MPI_INTEGER, MPI_SUM, dist%communicator, ierr)
      else
         global_count_dbl = 0
      endif
   else
      global_count_dbl = local_count
   endif

!-----------------------------------------------------------------------

 end function global_count_dbl

!***********************************************************************

 function global_count_real (MASK, dist, field_loc)

!-----------------------------------------------------------------------
!
!  this function returns the number of true or non-zero elements
!  in the physical domain of a 2-d array
!
!-----------------------------------------------------------------------

   include 'mpif.h'  ! MPI Fortran include file

   real (r4), dimension(:,:,:), intent(in) :: &
      MASK

   type (distrb), intent(in) :: &
      dist                 ! block distribution for MASK

   integer (int_kind), intent(in) :: &
      field_loc            ! location of field on staggered grid

   integer (int_kind) :: &
      global_count_real,    &! resulting global count
      local_count,         &! local count of subdomain
      ierr,                &! MPI error flag
      ib, ie, jb, je,      &! start,end of physical domain
      bid,                 &! block location in distribution
      i, n                  ! dummy counters

   type (block) :: &
      this_block          ! holds local block information

!-----------------------------------------------------------------------

   local_count = 0
   if (ltripole_grid .and. (field_loc == field_loc_Nface .or. &
                            field_loc == field_loc_NEcorner)) then
      !*** must remove redundant points from sum
      do n=1,nblocks_tot
         if (dist%proc(n) == my_task+1) then
            bid = dist%local_block(n)
            this_block = get_block(n,bid)
            ib = this_block%ib
            ie = this_block%ie
            jb = this_block%jb
            je = this_block%je
            if (this_block%jblock == nblocks_y) then
               !*** for topmost row in tripole only sum
               !*** 1st half of domain - others are redundant
               do i=ib,ie
                  if (this_block%i_glob(i) <= nx_global/2 .and. &
                      MASK(i,je,bid) /= c0) &
                     local_count = local_count + 1
               end do
               je = je - 1
            endif
            local_count = local_count &
                        + count(MASK(ib:ie,jb:je,bid) /= c0)
         endif
      end do !block loop
   else ! regular global count
      do n=1,nblocks_tot
         if (dist%proc(n) == my_task+1) then
            bid = dist%local_block(n)
            call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
            local_count = local_count &
                        + count(MASK(ib:ie,jb:je,bid) /= c0)
         endif
      end do !block loop
   endif

   if (dist%nprocs > 1) then
      if (my_task < dist%nprocs) then
         call MPI_ALLREDUCE(local_count, global_count_real, 1, &
                         MPI_INTEGER, MPI_SUM, dist%communicator, ierr)
      else
         global_count_real = 0
      endif
   else
      global_count_real = local_count
   endif

!-----------------------------------------------------------------------

 end function global_count_real

!***********************************************************************

 function global_count_int (MASK, dist, field_loc)

!-----------------------------------------------------------------------
!
!  this function returns the number of true or non-zero elements
!  in the physical domain of a 2-d array
!
!-----------------------------------------------------------------------

   include 'mpif.h'  ! MPI Fortran include file

   integer (int_kind), dimension(:,:,:), intent(in) :: &
      MASK

   type (distrb), intent(in) :: &
      dist                 ! block distribution for MASK

   integer (int_kind), intent(in) :: &
      field_loc            ! location of field on staggered grid

   integer (int_kind) :: &
      global_count_int,    &! resulting global count
      local_count,         &! local count of subdomain
      ierr,                &! MPI error flag
      ib, ie, jb, je,      &! start,end of physical domain
      bid,                 &! block location in distribution
      i, n                  ! dummy counters

   type (block) :: &
      this_block          ! holds local block information

!-----------------------------------------------------------------------

   local_count = 0
   if (ltripole_grid .and. (field_loc == field_loc_Nface .or. &
                            field_loc == field_loc_NEcorner)) then
      !*** must remove redundant points from sum
      do n=1,nblocks_tot
         if (dist%proc(n) == my_task+1) then
            bid = dist%local_block(n)
            this_block = get_block(n,bid)
            ib = this_block%ib
            ie = this_block%ie
            jb = this_block%jb
            je = this_block%je
            if (this_block%jblock == nblocks_y) then
               !*** for topmost row in tripole only sum
               !*** 1st half of domain - others are redundant
               do i=ib,ie
                  if (this_block%i_glob(i) <= nx_global/2 .and. &
                      MASK(i,je,bid) /= 0) &
                     local_count = local_count + 1
               end do
               je = je - 1
            endif
            local_count = local_count &
                        + count(MASK(ib:ie,jb:je,bid) /= 0)
         endif
      end do !block loop
   else ! regular global count
      do n=1,nblocks_tot
         if (dist%proc(n) == my_task+1) then
            bid = dist%local_block(n)
            call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
            local_count = local_count &
                        + count(MASK(ib:ie,jb:je,bid) /= 0)
         endif
      end do !block loop
   endif

   if (dist%nprocs > 1) then
      if (my_task < dist%nprocs) then
         call MPI_ALLREDUCE(local_count, global_count_int, 1, &
                         MPI_INTEGER, MPI_SUM, dist%communicator, ierr)
      else
         global_count_int = 0
      endif
   else
      global_count_int = local_count
   endif

!-----------------------------------------------------------------------

 end function global_count_int

!***********************************************************************

 function global_count_log (MASK, dist, field_loc)

!-----------------------------------------------------------------------
!
!  this function returns the number of true or non-zero elements
!  in the physical domain of a 2-d array
!
!-----------------------------------------------------------------------

   include 'mpif.h'  ! MPI Fortran include file

   logical (log_kind), dimension(:,:,:), intent(in) :: &
      MASK

   type (distrb), intent(in) :: &
      dist                 ! block distribution for MASK

   integer (int_kind), intent(in) :: &
      field_loc            ! location of field on staggered grid

   integer (int_kind) :: &
      global_count_log     ! resulting global count

   integer (int_kind) :: &
      local_count,         &! local count of subdomain
      ierr,                &! MPI error flag
      ib, ie, jb, je,      &! start,end of physical domain
      bid,                 &! block location in distribution
      i, n                  ! dummy counters

   type (block) :: &
      this_block          ! holds local block information

!-----------------------------------------------------------------------

   local_count = 0
   if (ltripole_grid .and. (field_loc == field_loc_Nface .or. &
                            field_loc == field_loc_NEcorner)) then
      !*** must remove redundant points from sum
      do n=1,nblocks_tot
         if (dist%proc(n) == my_task+1) then
            bid = dist%local_block(n)
            this_block = get_block(n,bid)
            ib = this_block%ib
            ie = this_block%ie
            jb = this_block%jb
            je = this_block%je
            if (this_block%jblock == nblocks_y) then
               !*** for topmost row in tripole only sum
               !*** 1st half of domain - others are redundant
               do i=ib,ie
                  if (this_block%i_glob(i) <= nx_global/2 .and. &
                      MASK(i,je,bid)) &
                     local_count = local_count + 1
               end do
               je = je - 1
            endif
            local_count = local_count &
                        + count(MASK(ib:ie,jb:je,bid))
         endif
      end do !block loop
   else ! regular global count
      do n=1,nblocks_tot
         if (dist%proc(n) == my_task+1) then
            bid = dist%local_block(n)
            call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
            local_count = local_count &
                        + count(MASK(ib:ie,jb:je,bid))
         endif
      end do !block loop
   endif

   if (dist%nprocs > 1) then
      if (my_task < dist%nprocs) then
         call MPI_ALLREDUCE(local_count, global_count_log, 1, &
                         MPI_INTEGER, MPI_SUM, dist%communicator, ierr)
      else
         global_count_log = 0
      endif
   else
      global_count_log = local_count
   endif

!-----------------------------------------------------------------------
!EOC

 end function global_count_log

!***********************************************************************
!BOP
! !IROUTINE: global_maxval
! !INTERFACE:

 function global_maxval_dbl (X, dist, field_loc, LMASK)

! !DESCRIPTION:
!  This function computes the global maxval of the physical domain
!  of a 2-d field
!
! !REVISION HISTORY:
!  same as module
!
! !REMARKS:
!  This is actually the specific interface for the generic global_maxval
!  function corresponding to double precision arrays.  The generic
!  interface is identical but will handle real and integer 2-d slabs.

! !USES:

   include 'mpif.h'  ! MPI Fortran include file

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  input vars
!
!-----------------------------------------------------------------------

   real (r8), dimension(:,:,:), intent(in) :: &
      X            ! array containing field for which max required

   type (distrb), intent(in) :: &
      dist                 ! block distribution for array X

   integer (int_kind), intent(in) :: &
      field_loc            ! location of field on staggered grid

   logical (log_kind), &
     dimension(size(X,dim=1),size(X,dim=2),size(X,dim=3)), &
     intent(in), optional :: &
      LMASK                ! mask for excluding parts of domain

!-----------------------------------------------------------------------
!
!  output result
!
!-----------------------------------------------------------------------

   real (r8) :: &
      global_maxval_dbl   ! resulting max val of global domain

!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   real (r8) :: &
      local_maxval         ! max value of local subdomain

   integer (int_kind) :: &
      i,j,n,bid, &
      ib, ie, jb, je,     &! start,end of physical domain
      ierr                 ! MPI error flag

!-----------------------------------------------------------------------

   local_maxval = -bignum
   do n=1,nblocks_tot
      if (dist%proc(n) == my_task+1) then
         bid = dist%local_block(n)
         call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
         if (present(LMASK)) then
            do j=jb,je
            do i=ib,ie
               if (LMASK(i,j,bid)) &
                  local_maxval = max(local_maxval,X(i,j,bid))
            end do
            end do
         else ! no mask
            do j=jb,je
            do i=ib,ie
               local_maxval = max(local_maxval,X(i,j,bid))
            end do
            end do
         endif
      endif
   end do ! block loop

   if (dist%nprocs > 1) then
      if (my_task < dist%nprocs) then
         call MPI_ALLREDUCE(local_maxval, global_maxval_dbl, 1, &
                            mpi_dbl, MPI_MAX, dist%communicator, ierr)
      else
         global_maxval_dbl = c0
      endif
   else
      global_maxval_dbl = local_maxval
   endif

!-----------------------------------------------------------------------

 end function global_maxval_dbl

!***********************************************************************

 function global_maxval_real (X, dist, field_loc, LMASK)

!-----------------------------------------------------------------------
!
!  this function computes the global maxval of the physical domain
!  of a 2-d field
!
!-----------------------------------------------------------------------

   include 'mpif.h'  ! MPI Fortran include file

!-----------------------------------------------------------------------
!
!  input vars
!
!-----------------------------------------------------------------------

   real (r4), dimension(:,:,:), intent(in) :: &
      X            ! array containing field for which max required

   type (distrb), intent(in) :: &
      dist                 ! block distribution for array X

   integer (int_kind), intent(in) :: &
      field_loc            ! location of field on staggered grid

   logical (log_kind), &
     dimension(size(X,dim=1),size(X,dim=2),size(X,dim=3)), &
     intent(in), optional :: &
      LMASK                ! mask for excluding parts of domain

!-----------------------------------------------------------------------
!
!  output result
!
!-----------------------------------------------------------------------

   real (r4) :: &
      global_maxval_real   ! resulting max val of global domain

!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   real (r4) :: &
      local_maxval         ! max value of local subdomain

   integer (int_kind) :: &
      i,j,n,bid, &
      ib, ie, jb, je,     &! start,end of physical domain
      ierr                 ! MPI error flag

!-----------------------------------------------------------------------

   local_maxval = -bignum
   do n=1,nblocks_tot
      if (dist%proc(n) == my_task+1) then
         bid = dist%local_block(n)
         call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
         if (present(LMASK)) then
            do j=jb,je
            do i=ib,ie
               if (LMASK(i,j,bid)) &
                  local_maxval = max(local_maxval,X(i,j,bid))
            end do
            end do
         else ! no mask
            do j=jb,je
            do i=ib,ie
               local_maxval = max(local_maxval,X(i,j,bid))
            end do
            end do
         endif
      endif
   end do ! block loop

   if (dist%nprocs > 1) then
      if (my_task < dist%nprocs) then
         call MPI_ALLREDUCE(local_maxval, global_maxval_real, 1, &
                            mpi_real, MPI_MAX, dist%communicator, ierr)
      else
         global_maxval_real = c0
      endif
   else
      global_maxval_real = local_maxval
   endif

!-----------------------------------------------------------------------

 end function global_maxval_real

!***********************************************************************

 function global_maxval_int (X, dist, field_loc, LMASK)

!-----------------------------------------------------------------------
!
!  this function computes the global maxval of the physical domain
!  of a 2-d field
!
!-----------------------------------------------------------------------

   include 'mpif.h'  ! MPI Fortran include file

!-----------------------------------------------------------------------
!
!  input vars
!
!-----------------------------------------------------------------------

   integer (int_kind), dimension(:,:,:), intent(in) :: &
      X            ! array containing field for which max required

   type (distrb), intent(in) :: &
      dist                 ! block distribution for array X

   integer (int_kind), intent(in) :: &
      field_loc            ! location of field on staggered grid

   logical (log_kind), &
     dimension(size(X,dim=1),size(X,dim=2),size(X,dim=3)), &
     intent(in), optional :: &
      LMASK                ! mask for excluding parts of domain

!-----------------------------------------------------------------------
!
!  output result
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      global_maxval_int    ! resulting max val of global domain

!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      local_maxval         ! max value of local subdomain

   integer (int_kind) :: &
      i,j,n,bid, &
      ib, ie, jb, je,     &! start,end of physical domain
      ierr                 ! MPI error flag

!-----------------------------------------------------------------------

   local_maxval = -1000000
   do n=1,nblocks_tot
      if (dist%proc(n) == my_task+1) then
         bid = dist%local_block(n)
         call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
         if (present(LMASK)) then
            do j=jb,je
            do i=ib,ie
               if (LMASK(i,j,bid)) &
                  local_maxval = max(local_maxval,X(i,j,bid))
            end do
            end do
         else ! no mask
            do j=jb,je
            do i=ib,ie
               local_maxval = max(local_maxval,X(i,j,bid))
            end do
            end do
         endif
      endif
   end do ! block loop

   if (dist%nprocs > 1) then
      if (my_task < dist%nprocs) then
         call MPI_ALLREDUCE(local_maxval, global_maxval_int, 1, &
                         mpi_integer, MPI_MAX, dist%communicator, ierr)
      else
         global_maxval_int = 0
      endif
   else
      global_maxval_int = local_maxval
   endif

!-----------------------------------------------------------------------
!EOC

 end function global_maxval_int

!***********************************************************************
!BOP
! !IROUTINE: global_minval
! !INTERFACE:

 function global_minval_dbl (X, dist, field_loc, LMASK)

! !DESCRIPTION:
!  This function computes the global minval of the physical domain
!  of a 2-d field
!
! !REVISION HISTORY:
!  same as module
!
! !REMARKS:
!  This is actually the specific interface for the generic global_minval
!  function corresponding to double precision arrays.  The generic
!  interface is identical but will handle real and integer 2-d slabs.

! !USES:

   include 'mpif.h'  ! MPI Fortran include file

! !INPUT PARAMETERS:

   real (r8), dimension(:,:,:), intent(in) :: &
      X            ! array containing field for which min required

   type (distrb), intent(in) :: &
      dist                 ! block distribution for array X

   integer (int_kind), intent(in) :: &
      field_loc            ! location of field on staggered grid

   logical (log_kind), &
     dimension(size(X,dim=1),size(X,dim=2),size(X,dim=3)), &
     intent(in), optional :: &
      LMASK                ! mask for excluding parts of domain

! !OUTPUT PARAMETERS:

   real (r8) :: &
      global_minval_dbl   ! resulting min val of global domain

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   real (r8) :: &
      local_minval         ! min value of local subdomain

   integer (int_kind) :: &
      i,j,n,bid, &
      ib, ie, jb, je,     &! start,end of physical domain
      ierr                 ! MPI error flag

!-----------------------------------------------------------------------

   local_minval = bignum
   do n=1,nblocks_tot
      if (dist%proc(n) == my_task+1) then
         bid = dist%local_block(n)
         call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
         if (present(LMASK)) then
            do j=jb,je
            do i=ib,ie
               if (LMASK(i,j,bid)) &
                  local_minval = min(local_minval,X(i,j,bid))
            end do
            end do
         else ! no mask
            do j=jb,je
            do i=ib,ie
               local_minval = min(local_minval,X(i,j,bid))
            end do
            end do
         endif
      endif
   end do

   if (dist%nprocs > 1) then
      if (my_task < dist%nprocs) then
         call MPI_ALLREDUCE(local_minval, global_minval_dbl, 1, &
                            mpi_dbl, MPI_MIN, dist%communicator, ierr)
      else
         global_minval_dbl = c0
      endif
   else
      global_minval_dbl = local_minval
   endif

!-----------------------------------------------------------------------

 end function global_minval_dbl

!***********************************************************************

 function global_minval_real (X, dist, field_loc, LMASK)

!-----------------------------------------------------------------------
!
!  this function computes the global minval of the physical domain
!  of a 2-d field
!
!-----------------------------------------------------------------------

   include 'mpif.h'  ! MPI Fortran include file

!-----------------------------------------------------------------------
!
!  input vars
!
!-----------------------------------------------------------------------

   real (r4), dimension(:,:,:), intent(in) :: &
      X            ! array containing field for which min required

   type (distrb), intent(in) :: &
      dist                 ! block distribution for array X

   integer (int_kind), intent(in) :: &
      field_loc            ! location of field on staggered grid

   logical (log_kind), &
     dimension(size(X,dim=1),size(X,dim=2),size(X,dim=3)), &
     intent(in), optional :: &
      LMASK                ! mask for excluding parts of domain

!-----------------------------------------------------------------------
!
!  output result
!
!-----------------------------------------------------------------------

   real (r4) :: &
      global_minval_real   ! resulting min val of global domain

!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   real (r4) :: &
      local_minval         ! min value of local subdomain

   integer (int_kind) :: &
      i,j,n,bid, &
      ib, ie, jb, je,     &! start,end of physical domain
      ierr                 ! MPI error flag

!-----------------------------------------------------------------------

   local_minval = bignum
   do n=1,nblocks_tot
      if (dist%proc(n) == my_task+1) then
         bid = dist%local_block(n)
         call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
         if (present(LMASK)) then
            do j=jb,je
            do i=ib,ie
               if (LMASK(i,j,bid)) &
                  local_minval = min(local_minval,X(i,j,bid))
            end do
            end do
         else ! no mask
            do j=jb,je
            do i=ib,ie
               local_minval = min(local_minval,X(i,j,bid))
            end do
            end do
         endif
      endif
   end do ! block loop

   if (dist%nprocs > 1) then
      if (my_task < dist%nprocs) then
         call MPI_ALLREDUCE(local_minval, global_minval_real, 1, &
                            mpi_real, MPI_MIN, dist%communicator, ierr)
      else
         global_minval_real = c0
      endif
   else
      global_minval_real = local_minval
   endif

!-----------------------------------------------------------------------

 end function global_minval_real

!***********************************************************************

 function global_minval_int (X, dist, field_loc, LMASK)

!-----------------------------------------------------------------------
!
!  this function computes the global minval of the physical domain
!  of a 2-d field
!
!-----------------------------------------------------------------------

   include 'mpif.h'  ! MPI Fortran include file

!-----------------------------------------------------------------------
!
!  input vars
!
!-----------------------------------------------------------------------

   integer (int_kind), dimension(:,:,:), intent(in) :: &
      X            ! array containing field for which min required

   type (distrb), intent(in) :: &
      dist                 ! block distribution for array X

   integer (int_kind), intent(in) :: &
      field_loc            ! location of field on staggered grid

   logical (log_kind), &
     dimension(size(X,dim=1),size(X,dim=2),size(X,dim=3)), &
     intent(in), optional :: &
      LMASK                ! mask for excluding parts of domain

!-----------------------------------------------------------------------
!
!  output result
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      global_minval_int    ! resulting min val of global domain

!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      local_minval         ! min value of local subdomain

   integer (int_kind) :: &
      i,j,n,bid, &
      ib, ie, jb, je,     &! start,end of physical domain
      ierr                 ! MPI error flag

!-----------------------------------------------------------------------

   local_minval = 1000000
   do n=1,nblocks_tot
      if (dist%proc(n) == my_task+1) then
         bid = dist%local_block(n)
         call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
         if (present(LMASK)) then
            do j=jb,je
            do i=ib,ie
               if (LMASK(i,j,bid)) &
                  local_minval = min(local_minval,X(i,j,bid))
            end do
            end do
         else ! no mask
            do j=jb,je
            do i=ib,ie
               local_minval = min(local_minval,X(i,j,bid))
            end do
            end do
         endif
      endif
   end do

   if (dist%nprocs > 1) then
      if (my_task < dist%nprocs) then
         call MPI_ALLREDUCE(local_minval, global_minval_int, 1, &
                         mpi_integer, MPI_MIN, dist%communicator, ierr)
      else
         global_minval_int = 0
      endif
   else
      global_minval_int = local_minval
   endif

!-----------------------------------------------------------------------

 end function global_minval_int

!***********************************************************************

 function global_maxval_scalar_dbl (local_scalar)

!-----------------------------------------------------------------------
!
!  this function returns the maximum scalar value across processors
!
!-----------------------------------------------------------------------

   include 'mpif.h'  ! MPI Fortran include file

   real (r8), intent(inout) :: &
      local_scalar                ! local scalar to be compared

   real (r8) :: &
      global_maxval_scalar_dbl   ! resulting global max

   integer (int_kind) :: ierr ! MPI error flag

!-----------------------------------------------------------------------

   call MPI_ALLREDUCE(local_scalar, global_maxval_scalar_dbl, 1, &
                      mpi_dbl, MPI_MAX, MPI_COMM_OCN, ierr)

!-----------------------------------------------------------------------

 end function global_maxval_scalar_dbl

!***********************************************************************

 function global_maxval_scalar_real (local_scalar)

!-----------------------------------------------------------------------
!
!  this function returns the maximum scalar value across processors
!
!-----------------------------------------------------------------------

   include 'mpif.h'  ! MPI Fortran include file

   real (r4), intent(inout) :: &
      local_scalar                ! local scalar to be compared

   real (r4) :: &
      global_maxval_scalar_real   ! resulting global max

   integer (int_kind) :: ierr ! MPI error flag

!-----------------------------------------------------------------------

   call MPI_ALLREDUCE(local_scalar, global_maxval_scalar_real, 1, &
                      MPI_REAL, MPI_MAX, MPI_COMM_OCN, ierr)

!-----------------------------------------------------------------------

 end function global_maxval_scalar_real

!***********************************************************************

 function global_maxval_scalar_int (local_scalar)

!-----------------------------------------------------------------------
!
!  this function returns the maximum scalar value across processors
!
!-----------------------------------------------------------------------

   include 'mpif.h'  ! MPI Fortran include file

   integer (int_kind), intent(inout) :: &
      local_scalar                ! local scalar to be compared

   integer (int_kind) :: &
      global_maxval_scalar_int   ! resulting global max

   integer (int_kind) :: ierr ! MPI error flag

!-----------------------------------------------------------------------

   call MPI_ALLREDUCE(local_scalar, global_maxval_scalar_int, 1, &
                      MPI_INTEGER, MPI_MAX, MPI_COMM_OCN, ierr)

!-----------------------------------------------------------------------

 end function global_maxval_scalar_int

!***********************************************************************

 function global_minval_scalar_dbl (local_scalar)

!-----------------------------------------------------------------------
!
!  this function returns the minimum scalar value across processors
!
!-----------------------------------------------------------------------

   include 'mpif.h'  ! MPI Fortran include file

   real (r8), intent(inout) :: &
      local_scalar                ! local scalar to be compared

   real (r8) :: &
      global_minval_scalar_dbl   ! resulting global min

   integer (int_kind) :: ierr ! MPI error flag

!-----------------------------------------------------------------------

   call MPI_ALLREDUCE(local_scalar, global_minval_scalar_dbl, 1, &
                      mpi_dbl, MPI_MIN, MPI_COMM_OCN, ierr)

!-----------------------------------------------------------------------

 end function global_minval_scalar_dbl

!***********************************************************************

 function global_minval_scalar_real (local_scalar)

!-----------------------------------------------------------------------
!
!  this function returns the minimum scalar value across processors
!
!-----------------------------------------------------------------------

   include 'mpif.h'  ! MPI Fortran include file

   real (r4), intent(inout) :: &
      local_scalar                ! local scalar to be compared

   real (r4) :: &
      global_minval_scalar_real   ! resulting global min

   integer (int_kind) :: ierr ! MPI error flag

!-----------------------------------------------------------------------

   call MPI_ALLREDUCE(local_scalar, global_minval_scalar_real, 1, &
                      MPI_REAL, MPI_MIN, MPI_COMM_OCN, ierr)

!-----------------------------------------------------------------------

 end function global_minval_scalar_real

!***********************************************************************

 function global_minval_scalar_int (local_scalar)

!-----------------------------------------------------------------------
!
!  this function returns the minimum scalar value across processors
!
!-----------------------------------------------------------------------

   include 'mpif.h'  ! MPI Fortran include file

   integer (int_kind), intent(inout) :: &
      local_scalar                ! local scalar to be compared

   integer (int_kind) :: &
      global_minval_scalar_int   ! resulting global min

   integer (int_kind) :: ierr ! MPI error flag

!-----------------------------------------------------------------------

   call MPI_ALLREDUCE(local_scalar, global_minval_scalar_int, 1, &
                      MPI_INTEGER, MPI_MIN, MPI_COMM_OCN, ierr)

!-----------------------------------------------------------------------
!EOC

 end function global_minval_scalar_int

!***********************************************************************
!BOP
! !IROUTINE: global_maxloc
! !INTERFACE:

 subroutine global_maxloc_dbl(WORK,dist,field_loc,ival,jval,val)

! !DESCRIPTION:
!  This routine finds the location of the global maximum for the
!  physical domain of a 2-d field
!
! !REVISION HISTORY:
!  same as module
!
! !REMARKS:
!  This is actually the specific interface for the generic global_maxval
!  function corresponding to double precision arrays.  The generic
!  interface is identical but will handle real and integer 2-d slabs.

! !USES:

   include 'mpif.h'  ! MPI Fortran include file

! !INPUT PARAMETERS:

   real (r8), dimension(:,:,:), intent(in) :: &
       WORK             ! array for which maxloc required

   type (distrb), intent(in) :: &
      dist                 ! block distribution for array X

   integer (int_kind), intent(in) :: &
      field_loc            ! location of field on staggered grid

! !OUTPUT PARAMETERS:

   real (r8), intent(out) :: &
       val              ! maximum value of the field

   integer (int_kind), intent(out) :: &
       ival, jval       ! global i,j location of maximum

!EOP
!BOC

   integer (int_kind) :: &
       i, j, n, bid,   &! dummy counters
       ierr             ! MPI error flag

   real (r8), dimension(3) :: &
       local_val        ! local array to hold local i,j,val

   real (r8), dimension(:), allocatable :: &
       global_val       ! array to hold gathered i,j,val from others

   type (block) :: &
       this_block       ! current block info

!-----------------------------------------------------------------------

   !***
   !*** find local maxval with global location
   !***

   val = -bignum
   do n=1,nblocks_tot
     if (dist%proc(n) == my_task+1) then
       bid = dist%local_block(n)
       this_block = get_block(n,bid)
       do j=this_block%jb,this_block%je
       do i=this_block%ib,this_block%ie
         if (WORK(i,j,bid) > val) then
           val = WORK(i,j,bid)
           local_val(1) = val
           local_val(2) = this_block%i_glob(i)
           local_val(3) = this_block%j_glob(j)
         endif
       end do
       end do
     endif
   end do

   !***
   !*** gather all similar results from other processors
   !***

   allocate (global_val(0:(3*dist%nprocs)-1))

   if (dist%nprocs > 1) then 
      if (my_task < dist%nprocs) then
         call MPI_ALLGATHER(local_val , 3, mpi_dbl, &
                            global_val, 3, mpi_dbl, &
                            dist%communicator, ierr)
      else
         global_val = c0
      endif
   else
      global_val(0:2) = local_val(1:3)
   endif

   !***
   !*** find max and location by comparing the gathered results
   !***

   val  = -bignum
   ival = 0
   jval = 0
   do n=0,3*dist%nprocs-1,3
     if (global_val(n) > val) then
       val  = global_val(n  )
       ival = global_val(n+1)
       jval = global_val(n+2)
     end if
   end do

   deallocate(global_val)

!-----------------------------------------------------------------------

 end subroutine global_maxloc_dbl

!***********************************************************************

 subroutine global_maxloc_real(WORK,dist,field_loc,ival,jval,val)

!-----------------------------------------------------------------------
!
!  this routine finds the location of the global maximum for the
!  physical domain of a 2-d field
!
!-----------------------------------------------------------------------

   include 'mpif.h'  ! MPI Fortran include file

   real (r4), dimension(:,:,:), intent(in) :: &
       WORK             ! array for which maxloc required

   type (distrb), intent(in) :: &
      dist                 ! block distribution for array X

   integer (int_kind), intent(in) :: &
      field_loc            ! location of field on staggered grid

   real (r4), intent(out) :: &
       val              ! maximum value of the field

   integer (int_kind), intent(out) :: &
       ival, jval       ! i,j location of maximum

   integer (int_kind) :: &
       i, j, n, bid,   &! dummy counters
       ierr             ! MPI error flag

   real (r4), dimension(3) :: &
       local_val        ! local array to hold local i,j,val

   real (r4), dimension(:), allocatable :: &
       global_val       ! array to hold gathered i,j,val from others

   type (block) :: &
       this_block       ! current block info

!-----------------------------------------------------------------------

   !***
   !*** find local maxval with global location
   !***

   val = -bignum
   do n=1,nblocks_tot
     if (dist%proc(n) == my_task+1) then
       bid = dist%local_block(n)
       this_block = get_block(n,bid)
       do j=this_block%jb,this_block%je
       do i=this_block%ib,this_block%ie
         if (WORK(i,j,bid) > val) then
           val = WORK(i,j,bid)
           local_val(1) = val
           local_val(2) = this_block%i_glob(i)
           local_val(3) = this_block%j_glob(j)
         endif
       end do
       end do
     endif
   end do

   !***
   !*** gather all similar results from other processors
   !***

   allocate (global_val(0:(3*dist%nprocs)-1))

   if (dist%nprocs > 1) then 
      if (my_task < dist%nprocs) then
         call MPI_ALLGATHER(local_val , 3, mpi_real, &
                            global_val, 3, mpi_real, &
                            dist%communicator, ierr)
      else
         global_val = c0
      endif
   else
      global_val(0:2) = local_val(1:3)
   endif

   !***
   !*** find max and location by comparing the gathered results
   !***

   val  = -bignum
   ival = 0
   jval = 0
   do n=0,3*dist%nprocs-1,3
     if (global_val(n) > val) then
       val  = global_val(n  )
       ival = global_val(n+1)
       jval = global_val(n+2)
     end if
   end do

   deallocate(global_val)

!-----------------------------------------------------------------------

 end subroutine global_maxloc_real

!***********************************************************************

 subroutine global_maxloc_int(WORK,dist,field_loc,ival,jval,val)

!-----------------------------------------------------------------------
!
!  this routine finds the location of the global maximum for the
!  physical domain of a 2-d field
!
!-----------------------------------------------------------------------

   include 'mpif.h'  ! MPI Fortran include file

   integer (int_kind), dimension(:,:,:), intent(in) :: &
       WORK             ! array for which maxloc required

   type (distrb), intent(in) :: &
      dist                 ! block distribution for array X

   integer (int_kind), intent(in) :: &
      field_loc            ! location of field on staggered grid

   integer (int_kind), intent(out) :: &
       val              ! maximum value of the field

   integer (int_kind), intent(out) :: &
       ival, jval       ! i,j location of maximum

   integer (int_kind) :: &
       i, j, n, bid,   &! dummy counters
       ierr             ! MPI error flag

   integer (int_kind), dimension(3) :: &
       local_val        ! local array to hold local i,j,val

   integer (int_kind), dimension(:), allocatable :: &
       global_val       ! array to hold gathered i,j,val from others

   type (block) :: &
       this_block       ! current block info

!-----------------------------------------------------------------------

   !***
   !*** find local maxval with global location
   !***

   val = -1000000
   do n=1,nblocks_tot
     if (dist%proc(n) == my_task+1) then
       bid = dist%local_block(n)
       this_block = get_block(n,bid)
       do j=this_block%jb,this_block%je
       do i=this_block%ib,this_block%ie
         if (WORK(i,j,bid) > val) then
           val = WORK(i,j,bid)
           local_val(1) = val
           local_val(2) = this_block%i_glob(i)
           local_val(3) = this_block%j_glob(j)
         endif
       end do
       end do
     endif
   end do

   !***
   !*** gather all similar results from other processors
   !***

   allocate (global_val(0:(3*dist%nprocs)-1))

   if (dist%nprocs > 1) then 
      if (my_task < dist%nprocs) then
         call MPI_ALLGATHER(local_val , 3, mpi_integer, &
                            global_val, 3, mpi_integer, &
                            dist%communicator, ierr)
      else
         global_val = c0
      endif
   else
      global_val(0:2) = local_val(1:3)
   endif

   !***
   !*** find max and location by comparing the gathered results
   !***

   val  = -1000000
   ival = 0
   jval = 0
   do n=0,3*dist%nprocs-1,3
     if (global_val(n) > val) then
       val  = global_val(n  )
       ival = global_val(n+1)
       jval = global_val(n+2)
     end if
   end do

   deallocate(global_val)

!-----------------------------------------------------------------------
!EOC

 end subroutine global_maxloc_int

!***********************************************************************

 end module global_reductions

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
