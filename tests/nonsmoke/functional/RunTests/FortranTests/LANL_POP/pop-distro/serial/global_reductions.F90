!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module global_reductions

!BOP
! !MODULE: global_reductions
! !DESCRIPTION:
!  This module contains all the routines for performing global
!  reductions like global sums, minvals, maxvals, etc.
!
! !REVISION HISTORY:
!  CVS:$Id: global_reductions.F90,v 1.6 2004/01/07 19:56:38 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $

! !USES:

   use kinds_mod
   use communicate
   use constants
   use blocks
   use distribution
   use domain_size
   use domain

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

   real (r8) :: &
      local_block_sum      ! sum of local block domain

   integer (int_kind) :: &
      i,j,n,             &! local counters
      ib,ie,jb,je,       &! beg,end of physical domain
      bid                 ! block location

   type (block) :: &
      this_block          ! block information for local block

!-----------------------------------------------------------------------

   global_sum_dbl = c0

   if (ltripole_grid .and. (field_loc == field_loc_Nface .or. &
                            field_loc == field_loc_NEcorner)) then
      !*** must exclude redundant points
      do n=1,nblocks_tot
         if (dist%proc(n) /= 0) then
            bid = dist%local_block(n)
            this_block = get_block(n,bid)
            ib = this_block%ib
            ie = this_block%ie
            jb = this_block%jb
            je = this_block%je
            local_block_sum = c0
            if (this_block%jblock == nblocks_y) then
               !*** for the topmost row, half the points are
               !*** redundant so sum only the first half
               if (present(MASK)) then
                  do i=ib,ie
                     if (this_block%i_glob(i) <= nx_global/2) &
                        local_block_sum = &
                        local_block_sum + X(i,je,bid)*MASK(i,je,bid)
                  end do
               else ! no mask
                  do i=ib,ie
                     if (this_block%i_glob(i) <= nx_global/2) &
                        local_block_sum = local_block_sum + X(i,je,bid)
                  end do
               endif
               je = je - 1
            endif
            if (present(MASK)) then
               do j=jb,je
               do i=ib,ie
                  local_block_sum = &
                  local_block_sum + X(i,j,bid)*MASK(i,j,bid)
               end do
               end do
            else
               do j=jb,je
               do i=ib,ie
                  local_block_sum = &
                  local_block_sum + X(i,j,bid)
               end do
               end do
            endif
            global_sum_dbl = global_sum_dbl + local_block_sum
         endif
      end do
   else ! normal global sum
      do n=1,nblocks_tot
         if (dist%proc(n) /= 0) then
            bid = dist%local_block(n)
            call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
            local_block_sum = c0
            if (present(MASK)) then
               do j=jb,je
               do i=ib,ie
                  local_block_sum = &
                  local_block_sum + X(i,j,bid)*MASK(i,j,bid)
               end do
               end do
            else
               do j=jb,je
               do i=ib,ie
                  local_block_sum = &
                  local_block_sum + X(i,j,bid)
               end do
               end do
            endif
            global_sum_dbl = global_sum_dbl + local_block_sum
         endif
      end do
   endif

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

   real (r8) :: &
      local_block_sum      ! sum of local block domain

   integer (int_kind) :: &
      i,j,n,             &! local counters
      ib,ie,jb,je,       &! beg,end of physical domain
      bid                 ! block location

   type (block) :: &
      this_block          ! block information for local block

!-----------------------------------------------------------------------

   global_sum_real = c0

   if (ltripole_grid .and. (field_loc == field_loc_Nface .or. &
                            field_loc == field_loc_NEcorner)) then
      !*** must exclude redundant points
      do n=1,nblocks_tot
         if (dist%proc(n) /= 0) then
            bid = dist%local_block(n)
            this_block = get_block(n,bid)
            ib = this_block%ib
            ie = this_block%ie
            jb = this_block%jb
            je = this_block%je
            local_block_sum = c0
            if (this_block%jblock == nblocks_y) then
               !*** for the topmost row, half the points are
               !*** redundant so sum only the first half
               if (present(MASK)) then
                  do i=ib,ie
                     if (this_block%i_glob(i) <= nx_global/2) &
                        local_block_sum = &
                        local_block_sum + X(i,je,bid)*MASK(i,je,bid)
                  end do
               else ! no mask
                  do i=ib,ie
                     if (this_block%i_glob(i) <= nx_global/2) &
                        local_block_sum = local_block_sum + X(i,je,bid)
                  end do
               endif
               je = je - 1
            endif
            if (present(MASK)) then
               do j=jb,je
               do i=ib,ie
                  local_block_sum = &
                  local_block_sum + X(i,j,bid)*MASK(i,j,bid)
               end do
               end do
            else
               do j=jb,je
               do i=ib,ie
                  local_block_sum = &
                  local_block_sum + X(i,j,bid)
               end do
               end do
            endif
            global_sum_real = global_sum_real + local_block_sum
         endif
      end do
   else ! normal global sum
      do n=1,nblocks_tot
         if (dist%proc(n) /= 0) then
            bid = dist%local_block(n)
            call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
            local_block_sum = c0
            if (present(MASK)) then
               do j=jb,je
               do i=ib,ie
                  local_block_sum = &
                  local_block_sum + X(i,j,bid)*MASK(i,j,bid)
               end do
               end do
            else
               do j=jb,je
               do i=ib,ie
                  local_block_sum = &
                  local_block_sum + X(i,j,bid)
               end do
               end do
            endif
            global_sum_real = global_sum_real + local_block_sum
         endif
      end do
   endif

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

   integer (int_kind) :: &
      local_block_sum      ! sum of local block domain

   integer (int_kind) :: &
      i,j,n,             &! local counters
      ib,ie,jb,je,       &! beg,end of physical domain
      bid                 ! block location

   type (block) :: &
      this_block          ! block information for local block

!-----------------------------------------------------------------------

   global_sum_int = 0

   if (ltripole_grid .and. (field_loc == field_loc_Nface .or. &
                            field_loc == field_loc_NEcorner)) then
      !*** must exclude redundant points
      do n=1,nblocks_tot
         if (dist%proc(n) /= 0) then
            bid = dist%local_block(n)
            this_block = get_block(n,bid)
            ib = this_block%ib
            ie = this_block%ie
            jb = this_block%jb
            je = this_block%je
            local_block_sum = c0
            if (this_block%jblock == nblocks_y) then
               !*** for the topmost row, half the points are
               !*** redundant so sum only the first half
               if (present(MASK)) then
                  do i=ib,ie
                     if (this_block%i_glob(i) <= nx_global/2) &
                        local_block_sum = &
                        local_block_sum + X(i,je,bid)*MASK(i,je,bid)
                  end do
               else ! no mask
                  do i=ib,ie
                     if (this_block%i_glob(i) <= nx_global/2) &
                        local_block_sum = local_block_sum + X(i,je,bid)
                  end do
               endif
               je = je - 1
            endif
            if (present(MASK)) then
               do j=jb,je
               do i=ib,ie
                  local_block_sum = &
                  local_block_sum + X(i,j,bid)*MASK(i,j,bid)
               end do
               end do
            else
               do j=jb,je
               do i=ib,ie
                  local_block_sum = &
                  local_block_sum + X(i,j,bid)
               end do
               end do
            endif
            global_sum_int = global_sum_int + local_block_sum
         endif
      end do
   else ! normal global sum
      do n=1,nblocks_tot
         if (dist%proc(n) /= 0) then
            bid = dist%local_block(n)
            call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
            local_block_sum = c0
            if (present(MASK)) then
               do j=jb,je
               do i=ib,ie
                  local_block_sum = &
                  local_block_sum + X(i,j,bid)*MASK(i,j,bid)
               end do
               end do
            else
               do j=jb,je
               do i=ib,ie
                  local_block_sum = &
                  local_block_sum + X(i,j,bid)
               end do
               end do
            endif
            global_sum_int = global_sum_int + local_block_sum
         endif
      end do
   endif

!-----------------------------------------------------------------------

 end function global_sum_int

!***********************************************************************

 function global_sum_scalar_dbl(local_scalar, dist)

!-----------------------------------------------------------------------
!
!  this function returns the sum of scalar value across processors
!
!-----------------------------------------------------------------------

   type (distrb), intent(in) :: &
      dist                 ! distribution from which this is called

   real (r8), intent(in) :: &
      local_scalar                ! local scalar to be compared

   real (r8) :: &
      global_sum_scalar_dbl   ! sum of scalars across processors

!-----------------------------------------------------------------------
!
!  no operation needed for serial execution
!
!-----------------------------------------------------------------------

   global_sum_scalar_dbl = local_scalar

!-----------------------------------------------------------------------

 end function global_sum_scalar_dbl

!***********************************************************************

 function global_sum_scalar_real(local_scalar, dist)

!-----------------------------------------------------------------------
!
!  this function returns the sum of scalar value across processors
!
!-----------------------------------------------------------------------

   real (r4), intent(in) :: &
      local_scalar                ! local scalar to be compared

   type (distrb), intent(in) :: &
      dist                 ! distribution from which this is called

   real (r4) :: &
      global_sum_scalar_real   ! sum of scalars across processors

!-----------------------------------------------------------------------
!
!  no operation needed for serial execution
!
!-----------------------------------------------------------------------

   global_sum_scalar_real = local_scalar

!-----------------------------------------------------------------------

 end function global_sum_scalar_real

!***********************************************************************

 function global_sum_scalar_int(local_scalar, dist)

!-----------------------------------------------------------------------
!
!  this function returns the sum of scalar value across processors
!
!-----------------------------------------------------------------------

   integer (int_kind), intent(in) :: &
      local_scalar                ! local scalar to be compared

   type (distrb), intent(in) :: &
      dist                 ! distribution from which this is called

   integer (int_kind) ::      &
      global_sum_scalar_int   ! sum of scalars across processors

!-----------------------------------------------------------------------
!
!  no operation needed for serial execution
!
!-----------------------------------------------------------------------

   global_sum_scalar_int = local_scalar

!-----------------------------------------------------------------------

 end function global_sum_scalar_int

!EOC
!***********************************************************************
!BOP
! !IROUTINE: global_sum_prod
! !INTERFACE:

 function global_sum_prod_dbl (X, Y, dist, field_loc, MASK)

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

   real (r8) :: &
      local_block_sum      ! sum of local block domain

   integer (int_kind) :: &
      i,j,n,             &! local counters
      ib,ie,jb,je,       &! beg,end of physical domain
      bid                 ! block location

   type (block) :: &
      this_block          ! block information for local block

!-----------------------------------------------------------------------

   global_sum_prod_dbl = c0

   if (ltripole_grid .and. (field_loc == field_loc_Nface .or. &
                            field_loc == field_loc_NEcorner)) then
      !*** must exclude redundant points
      do n=1,nblocks_tot
         if (dist%proc(n) /= 0) then
            bid = dist%local_block(n)
            this_block = get_block(n,bid)
            ib = this_block%ib
            ie = this_block%ie
            jb = this_block%jb
            je = this_block%je
            local_block_sum = c0
            if (this_block%jblock == nblocks_y) then
               !*** for the topmost row, half the points are
               !*** redundant so sum only the first half
               if (present(MASK)) then
                  do i=ib,ie
                     if (this_block%i_glob(i) <= nx_global/2) &
                        local_block_sum = local_block_sum + &
                            X(i,je,bid)*Y(i,je,bid)*MASK(i,je,bid)
                  end do
               else ! no mask
                  do i=ib,ie
                     if (this_block%i_glob(i) <= nx_global/2) &
                        local_block_sum = local_block_sum + &
                                          X(i,je,bid)*Y(i,je,bid)
                  end do
               endif
               je = je - 1
            endif
            if (present(MASK)) then
               do j=jb,je
               do i=ib,ie
                  local_block_sum = &
                  local_block_sum + X(i,j,bid)*Y(i,j,bid)*MASK(i,j,bid)
               end do
               end do
            else
               do j=jb,je
               do i=ib,ie
                  local_block_sum = &
                  local_block_sum + X(i,j,bid)*Y(i,j,bid)
               end do
               end do
            endif
            global_sum_prod_dbl = global_sum_prod_dbl + local_block_sum
         endif
      end do
   else ! normal global sum
      do n=1,nblocks_tot
         if (dist%proc(n) /= 0) then
            bid = dist%local_block(n)
            call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
            local_block_sum = c0
            if (present(MASK)) then
               do j=jb,je
               do i=ib,ie
                  local_block_sum = &
                  local_block_sum + X(i,j,bid)*Y(i,j,bid)*MASK(i,j,bid)
               end do
               end do
            else
               do j=jb,je
               do i=ib,ie
                  local_block_sum = &
                  local_block_sum + X(i,j,bid)*Y(i,j,bid)
               end do
               end do
            endif
            global_sum_prod_dbl = global_sum_prod_dbl + local_block_sum
         endif
      end do
   endif

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

   real (r8) :: &
      local_block_sum      ! sum of local block domain

   integer (int_kind) :: &
      i,j,n,             &! local counters
      ib,ie,jb,je,       &! beg,end of physical domain
      bid                 ! block location

   type (block) :: &
      this_block          ! block information for local block

!-----------------------------------------------------------------------

   global_sum_prod_real = c0

   if (ltripole_grid .and. (field_loc == field_loc_Nface .or. &
                            field_loc == field_loc_NEcorner)) then
      !*** must exclude redundant points
      do n=1,nblocks_tot
         if (dist%proc(n) /= 0) then
            bid = dist%local_block(n)
            this_block = get_block(n,bid)
            ib = this_block%ib
            ie = this_block%ie
            jb = this_block%jb
            je = this_block%je
            local_block_sum = c0
            if (this_block%jblock == nblocks_y) then
               !*** for the topmost row, half the points are
               !*** redundant so sum only the first half
               if (present(MASK)) then
                  do i=ib,ie
                     if (this_block%i_glob(i) <= nx_global/2) &
                        local_block_sum = local_block_sum + &
                            X(i,je,bid)*Y(i,je,bid)*MASK(i,je,bid)
                  end do
               else ! no mask
                  do i=ib,ie
                     if (this_block%i_glob(i) <= nx_global/2) &
                        local_block_sum = local_block_sum + &
                                          X(i,je,bid)*Y(i,je,bid)
                  end do
               endif
               je = je - 1
            endif
            if (present(MASK)) then
               do j=jb,je
               do i=ib,ie
                  local_block_sum = &
                  local_block_sum + X(i,j,bid)*Y(i,j,bid)*MASK(i,j,bid)
               end do
               end do
            else
               do j=jb,je
               do i=ib,ie
                  local_block_sum = &
                  local_block_sum + X(i,j,bid)*Y(i,j,bid)
               end do
               end do
            endif
            global_sum_prod_real = global_sum_prod_real + local_block_sum
         endif
      end do
   else ! normal global sum
      do n=1,nblocks_tot
         if (dist%proc(n) /= 0) then
            bid = dist%local_block(n)
            call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
            local_block_sum = c0
            if (present(MASK)) then
               do j=jb,je
               do i=ib,ie
                  local_block_sum = &
                  local_block_sum + X(i,j,bid)*Y(i,j,bid)*MASK(i,j,bid)
               end do
               end do
            else
               do j=jb,je
               do i=ib,ie
                  local_block_sum = &
                  local_block_sum + X(i,j,bid)*Y(i,j,bid)
               end do
               end do
            endif
            global_sum_prod_real = global_sum_prod_real + local_block_sum
         endif
      end do
   endif

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

   integer (int_kind) :: &
      local_block_sum      ! sum of local block domain

   integer (int_kind) :: &
      i,j,n,             &! local counters
      ib,ie,jb,je,       &! beg,end of physical domain
      bid                 ! block location

   type (block) :: &
      this_block          ! block information for local block

!-----------------------------------------------------------------------

   global_sum_prod_int = 0

   if (ltripole_grid .and. (field_loc == field_loc_Nface .or. &
                            field_loc == field_loc_NEcorner)) then
      !*** must exclude redundant points
      do n=1,nblocks_tot
         if (dist%proc(n) /= 0) then
            bid = dist%local_block(n)
            this_block = get_block(n,bid)
            ib = this_block%ib
            ie = this_block%ie
            jb = this_block%jb
            je = this_block%je
            local_block_sum = c0
            if (this_block%jblock == nblocks_y) then
               !*** for the topmost row, half the points are
               !*** redundant so sum only the first half
               if (present(MASK)) then
                  do i=ib,ie
                     if (this_block%i_glob(i) <= nx_global/2) &
                        local_block_sum = local_block_sum + &
                            X(i,je,bid)*Y(i,je,bid)*MASK(i,je,bid)
                  end do
               else ! no mask
                  do i=ib,ie
                     if (this_block%i_glob(i) <= nx_global/2) &
                        local_block_sum = local_block_sum + &
                                          X(i,je,bid)*Y(i,je,bid)
                  end do
               endif
               je = je - 1
            endif
            if (present(MASK)) then
               do j=jb,je
               do i=ib,ie
                  local_block_sum = &
                  local_block_sum + X(i,j,bid)*Y(i,j,bid)*MASK(i,j,bid)
               end do
               end do
            else
               do j=jb,je
               do i=ib,ie
                  local_block_sum = &
                  local_block_sum + X(i,j,bid)*Y(i,j,bid)
               end do
               end do
            endif
            global_sum_prod_int = global_sum_prod_int + local_block_sum
         endif
      end do
   else ! normal global sum
      do n=1,nblocks_tot
         if (dist%proc(n) /= 0) then
            bid = dist%local_block(n)
            call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
            local_block_sum = c0
            if (present(MASK)) then
               do j=jb,je
               do i=ib,ie
                  local_block_sum = &
                  local_block_sum + X(i,j,bid)*Y(i,j,bid)*MASK(i,j,bid)
               end do
               end do
            else
               do j=jb,je
               do i=ib,ie
                  local_block_sum = &
                  local_block_sum + X(i,j,bid)*Y(i,j,bid)
               end do
               end do
            endif
            global_sum_prod_int = global_sum_prod_int + local_block_sum
         endif
      end do
   endif

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
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      ib, ie, jb, je,    &! start,end of physical domain
      bid,               &! block location in distribution
      i, n                ! dummy counters

   type (block) :: &
      this_block          ! block information for local block

!-----------------------------------------------------------------------

   global_count_dbl = 0

   if (ltripole_grid .and. (field_loc == field_loc_Nface .or. &
                            field_loc == field_loc_NEcorner)) then
      !*** must exclude redundant points
      do n=1,nblocks_tot
         if (dist%proc(n) /= 0) then
            bid = dist%local_block(n)
            this_block = get_block(n,bid)
            ib = this_block%ib
            ie = this_block%ie
            jb = this_block%jb
            je = this_block%je
            if (this_block%jblock == nblocks_y) then
               !*** for the topmost row, half the points are
               !*** redundant so count only the first half
               do i=ib,ie
                  if (this_block%i_glob(i) <= nx_global/2 .and. &
                      MASK(i,je,bid) /= c0) &
                     global_count_dbl = global_count_dbl + 1
               end do
               je = je - 1
            endif
            global_count_dbl = global_count_dbl &
                             + count(MASK(ib:ie,jb:je,bid) /= c0)
         endif
      end do
   else ! normal global sum
      do n=1,nblocks_tot
         if (dist%proc(n) /= 0) then
            bid = dist%local_block(n)
            call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
            global_count_dbl = global_count_dbl &
                             + count(MASK(ib:ie,jb:je,bid) /= c0)
         endif
      end do
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

   real (r4), dimension(:,:,:), intent(in) :: &
      MASK

   type (distrb), intent(in) :: &
      dist                 ! block distribution for MASK

   integer (int_kind), intent(in) :: &
      field_loc            ! location of field on staggered grid

   integer (int_kind) :: &
      global_count_real   ! resulting global count

!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      ib, ie, jb, je,    &! start,end of physical domain
      bid,               &! block location in distribution
      i, n                ! dummy counters

   type (block) :: &
      this_block          ! block information for local block

!-----------------------------------------------------------------------

   global_count_real = 0

   if (ltripole_grid .and. (field_loc == field_loc_Nface .or. &
                            field_loc == field_loc_NEcorner)) then
      !*** must exclude redundant points
      do n=1,nblocks_tot
         if (dist%proc(n) /= 0) then
            bid = dist%local_block(n)
            this_block = get_block(n,bid)
            ib = this_block%ib
            ie = this_block%ie
            jb = this_block%jb
            je = this_block%je
            if (this_block%jblock == nblocks_y) then
               !*** for the topmost row, half the points are
               !*** redundant so count only the first half
               do i=ib,ie
                  if (this_block%i_glob(i) <= nx_global/2 .and. &
                      MASK(i,je,bid) /= c0) &
                     global_count_real = global_count_real + 1
               end do
               je = je - 1
            endif
            global_count_real = global_count_real &
                             + count(MASK(ib:ie,jb:je,bid) /= c0)
         endif
      end do
   else ! normal global sum
      do n=1,nblocks_tot
         if (dist%proc(n) /= 0) then
            bid = dist%local_block(n)
            call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
            global_count_real = global_count_real &
                             + count(MASK(ib:ie,jb:je,bid) /= c0)
         endif
      end do
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

   integer (int_kind), dimension(:,:,:), intent(in) :: &
      MASK

   type (distrb), intent(in) :: &
      dist                 ! block distribution for MASK

   integer (int_kind), intent(in) :: &
      field_loc            ! location of field on staggered grid

   integer (int_kind) :: &
      global_count_int   ! resulting global count

!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      ib, ie, jb, je,    &! start,end of physical domain
      bid,               &! block location in distribution
      i, n                ! dummy counters

   type (block) :: &
      this_block          ! block information for local block

!-----------------------------------------------------------------------

   global_count_int = 0

   if (ltripole_grid .and. (field_loc == field_loc_Nface .or. &
                            field_loc == field_loc_NEcorner)) then
      !*** must exclude redundant points
      do n=1,nblocks_tot
         if (dist%proc(n) /= 0) then
            bid = dist%local_block(n)
            this_block = get_block(n,bid)
            ib = this_block%ib
            ie = this_block%ie
            jb = this_block%jb
            je = this_block%je
            if (this_block%jblock == nblocks_y) then
               !*** for the topmost row, half the points are
               !*** redundant so count only the first half
               do i=ib,ie
                  if (this_block%i_glob(i) <= nx_global/2 .and. &
                      MASK(i,je,bid) /= 0) &
                     global_count_int = global_count_int + 1
               end do
               je = je - 1
            endif
            global_count_int = global_count_int &
                             + count(MASK(ib:ie,jb:je,bid) /= 0)
         endif
      end do
   else ! normal global sum
      do n=1,nblocks_tot
         if (dist%proc(n) /= 0) then
            bid = dist%local_block(n)
            call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
            global_count_int = global_count_int &
                             + count(MASK(ib:ie,jb:je,bid) /= 0)
         endif
      end do
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

   logical (log_kind), dimension(:,:,:), intent(in) :: &
      MASK

   type (distrb), intent(in) :: &
      dist                 ! block distribution for MASK

   integer (int_kind), intent(in) :: &
      field_loc            ! location of field on staggered grid

   integer (int_kind) :: &
      global_count_log     ! resulting global count

!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      ib, ie, jb, je,    &! start,end of physical domain
      bid,               &! block location in distribution
      i, n                ! dummy counters

   type (block) :: &
      this_block          ! block information for local block

!-----------------------------------------------------------------------

   global_count_log = 0

   if (ltripole_grid .and. (field_loc == field_loc_Nface .or. &
                            field_loc == field_loc_NEcorner)) then
      !*** must exclude redundant points
      do n=1,nblocks_tot
         if (dist%proc(n) /= 0) then
            bid = dist%local_block(n)
            this_block = get_block(n,bid)
            ib = this_block%ib
            ie = this_block%ie
            jb = this_block%jb
            je = this_block%je
            if (this_block%jblock == nblocks_y) then
               !*** for the topmost row, half the points are
               !*** redundant so count only the first half
               do i=ib,ie
                  if (this_block%i_glob(i) <= nx_global/2 .and. &
                      MASK(i,je,bid)) &
                     global_count_log = global_count_log + 1
               end do
               je = je - 1
            endif
            global_count_log = global_count_log &
                             + count(MASK(ib:ie,jb:je,bid))
         endif
      end do
   else ! normal global sum
      do n=1,nblocks_tot
         if (dist%proc(n) /= 0) then
            bid = dist%local_block(n)
            call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
            global_count_log = global_count_log &
                             + count(MASK(ib:ie,jb:je,bid))
         endif
      end do
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

   integer (int_kind) :: &
      i,j,n,bid,         &
      ib, ie, jb, je      ! start,end of physical domain

!-----------------------------------------------------------------------

   global_maxval_dbl = -bignum

   do n=1,nblocks_tot
      if (dist%proc(n) /= 0) then
         bid = dist%local_block(n)
         call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
         if (present(LMASK)) then
            do j=jb,je
            do i=ib,ie
               if (LMASK(i,j,bid)) then
                  global_maxval_dbl = max(global_maxval_dbl, &
                                          X(i,j,bid))
               endif
            end do
            end do
         else
            do j=jb,je
            do i=ib,ie
               global_maxval_dbl = max(global_maxval_dbl, &
                                       X(i,j,bid))
            end do
            end do
         endif
      endif
   end do

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

   integer (int_kind) :: &
      i,j,n,bid,         &
      ib, ie, jb, je      ! start,end of physical domain

!-----------------------------------------------------------------------

   global_maxval_real = -bignum

   do n=1,nblocks_tot
      if (dist%proc(n) /= 0) then
         bid = dist%local_block(n)
         call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
         if (present(LMASK)) then
            do j=jb,je
            do i=ib,ie
               if (LMASK(i,j,bid)) then
                  global_maxval_real = max(global_maxval_real, &
                                           X(i,j,bid))
               endif
            end do
            end do
         else
            do j=jb,je
            do i=ib,ie
               global_maxval_real = max(global_maxval_real, &
                                        X(i,j,bid))
            end do
            end do
         endif
      endif
   end do

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
      i,j,n,bid,         &
      ib, ie, jb, je      ! start,end of physical domain

!-----------------------------------------------------------------------

   global_maxval_int = -1000000

   do n=1,nblocks_tot
      if (dist%proc(n) /= 0) then
         bid = dist%local_block(n)
         call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
         if (present(LMASK)) then
            do j=jb,je
            do i=ib,ie
               if (LMASK(i,j,bid)) then
                  global_maxval_int = max(global_maxval_int, &
                                          X(i,j,bid))
               endif
            end do
            end do
         else
            do j=jb,je
            do i=ib,ie
               global_maxval_int = max(global_maxval_int, &
                                       X(i,j,bid))
            end do
            end do
         endif
      endif
   end do

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

   integer (int_kind) :: &
      i,j,n,bid,         &
      ib, ie, jb, je      ! start,end of physical domain

!-----------------------------------------------------------------------

   global_minval_dbl = bignum

   do n=1,nblocks_tot
      if (dist%proc(n) /= 0) then
         bid = dist%local_block(n)
         call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
         if (present(LMASK)) then
            do j=jb,je
            do i=ib,ie
               if (LMASK(i,j,bid)) then
                  global_minval_dbl = min(global_minval_dbl, &
                                          X(i,j,bid))
               endif
            end do
            end do
         else
            do j=jb,je
            do i=ib,ie
               global_minval_dbl = min(global_minval_dbl, &
                                       X(i,j,bid))
            end do
            end do
         endif
      endif
   end do

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

   integer (int_kind) :: &
      i,j,n,bid,         &
      ib, ie, jb, je      ! start,end of physical domain

!-----------------------------------------------------------------------

   global_minval_real = bignum

   do n=1,nblocks_tot
      if (dist%proc(n) /= 0) then
         bid = dist%local_block(n)
         call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
         if (present(LMASK)) then
            do j=jb,je
            do i=ib,ie
               if (LMASK(i,j,bid)) then
                  global_minval_real = min(global_minval_real, &
                                           X(i,j,bid))
               endif
            end do
            end do
         else
            do j=jb,je
            do i=ib,ie
               global_minval_real = min(global_minval_real, &
                                        X(i,j,bid))
            end do
            end do
         endif
      endif
   end do

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
      i,j,n,bid,         &
      ib, ie, jb, je      ! start,end of physical domain

!-----------------------------------------------------------------------

   global_minval_int = 1000000

   do n=1,nblocks_tot
      if (dist%proc(n) /= 0) then
         bid = dist%local_block(n)
         call get_block_parameter(n,ib=ib,ie=ie,jb=jb,je=je)
         if (present(LMASK)) then
            do j=jb,je
            do i=ib,ie
               if (LMASK(i,j,bid)) then
                  global_minval_int = min(global_minval_int, &
                                          X(i,j,bid))
               endif
            end do
            end do
         else
            do j=jb,je
            do i=ib,ie
               global_minval_int = min(global_minval_int, &
                                       X(i,j,bid))
            end do
            end do
         endif
      endif
   end do

!-----------------------------------------------------------------------

 end function global_minval_int

!***********************************************************************

 function global_maxval_scalar_dbl (local_scalar)

!-----------------------------------------------------------------------
!
!  this function returns the maximum scalar value across processors
!
!-----------------------------------------------------------------------

   real (r8), intent(inout) :: &
      local_scalar                ! local scalar to be compared

   real (r8) :: &
      global_maxval_scalar_dbl   ! resulting global max

!-----------------------------------------------------------------------
!
!  no operations required for serial execution - return input value
!
!-----------------------------------------------------------------------

   global_maxval_scalar_dbl = local_scalar

!-----------------------------------------------------------------------

 end function global_maxval_scalar_dbl

!***********************************************************************

 function global_maxval_scalar_real (local_scalar)

!-----------------------------------------------------------------------
!
!  this function returns the maximum scalar value across processors
!
!-----------------------------------------------------------------------

   real (r4), intent(inout) :: &
      local_scalar                ! local scalar to be compared

   real (r4) :: &
      global_maxval_scalar_real   ! resulting global max

!-----------------------------------------------------------------------
!
!  no operations required for serial execution - return input value
!
!-----------------------------------------------------------------------

   global_maxval_scalar_real = local_scalar

!-----------------------------------------------------------------------

 end function global_maxval_scalar_real

!***********************************************************************

 function global_maxval_scalar_int (local_scalar)

!-----------------------------------------------------------------------
!
!  this function returns the maximum scalar value across processors
!
!-----------------------------------------------------------------------

   integer (int_kind), intent(inout) :: &
      local_scalar                ! local scalar to be compared

   integer (int_kind) :: &
      global_maxval_scalar_int   ! resulting global max

!-----------------------------------------------------------------------
!
!  no operations required for serial execution - return input value
!
!-----------------------------------------------------------------------

   global_maxval_scalar_int = local_scalar

!-----------------------------------------------------------------------

 end function global_maxval_scalar_int

!***********************************************************************

 function global_minval_scalar_dbl (local_scalar)

!-----------------------------------------------------------------------
!
!  this function returns the minimum scalar value across processors
!
!-----------------------------------------------------------------------

   real (r8), intent(inout) :: &
      local_scalar                ! local scalar to be compared

   real (r8) :: &
      global_minval_scalar_dbl   ! resulting global min

!-----------------------------------------------------------------------
!
!  no operations required for serial execution - return input value
!
!-----------------------------------------------------------------------

   global_minval_scalar_dbl = local_scalar

!-----------------------------------------------------------------------

 end function global_minval_scalar_dbl

!***********************************************************************

 function global_minval_scalar_real (local_scalar)

!-----------------------------------------------------------------------
!
!  this function returns the minimum scalar value across processors
!
!-----------------------------------------------------------------------

   real (r4), intent(inout) :: &
      local_scalar                ! local scalar to be compared

   real (r4) :: &
      global_minval_scalar_real   ! resulting global min

!-----------------------------------------------------------------------
!
!  no operations required for serial execution - return input value
!
!-----------------------------------------------------------------------

   global_minval_scalar_real = local_scalar

!-----------------------------------------------------------------------

 end function global_minval_scalar_real

!***********************************************************************

 function global_minval_scalar_int (local_scalar)

!-----------------------------------------------------------------------
!
!  this function returns the minimum scalar value across processors
!
!-----------------------------------------------------------------------

   integer (int_kind), intent(inout) :: &
      local_scalar                ! local scalar to be compared

   integer (int_kind) :: &
      global_minval_scalar_int   ! resulting global min

!-----------------------------------------------------------------------
!
!  no operations required for serial execution - return input value
!
!-----------------------------------------------------------------------

   global_minval_scalar_int = local_scalar

!-----------------------------------------------------------------------

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
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
       i, j, n, bid      ! dummy counters

   type (block) :: &
       this_block       ! current block info

!-----------------------------------------------------------------------

   !***
   !*** find local maxval with global location
   !***

   val = -bignum
   ival = 0
   jval = 0
   do n=1,nblocks_tot
      if (dist%proc(n) /= 0) then
         bid = dist%local_block(n)
         this_block = get_block(n,bid)
         do j=this_block%jb,this_block%je
         do i=this_block%ib,this_block%ie
            if (WORK(i,j,bid) > val) then
               val = WORK(i,j,bid)
               ival = this_block%i_glob(i)
               jval = this_block%j_glob(j)
            endif
         end do
         end do
      endif
   end do

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

   real (r4), dimension(:,:,:), intent(in) :: &
       WORK             ! array for which maxloc required

   type (distrb), intent(in) :: &
      dist                 ! block distribution for array X

   integer (int_kind), intent(in) :: &
      field_loc            ! location of field on staggered grid

   real (r4), intent(out) :: &
       val              ! maximum value of the field

   integer (int_kind), intent(out) :: &
       ival, jval       ! global i,j location of maximum

!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
       i, j, n, bid      ! dummy counters

   type (block) :: &
       this_block       ! current block info

!-----------------------------------------------------------------------

   !***
   !*** find local maxval with global location
   !***

   val = -bignum
   ival = 0
   jval = 0
   do n=1,nblocks_tot
      if (dist%proc(n) /= 0) then
         bid = dist%local_block(n)
         this_block = get_block(n,bid)
         do j=this_block%jb,this_block%je
         do i=this_block%ib,this_block%ie
            if (WORK(i,j,bid) > val) then
               val = WORK(i,j,bid)
               ival = this_block%i_glob(i)
               jval = this_block%j_glob(j)
            endif
         end do
         end do
      endif
   end do

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

!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
       i, j, n, bid      ! dummy counters

   type (block) :: &
       this_block       ! current block info

!-----------------------------------------------------------------------

   !***
   !*** find local maxval with global location
   !***

   val = -1000000
   ival = 0
   jval = 0
   do n=1,nblocks_tot
      if (dist%proc(n) /= 0) then
         bid = dist%local_block(n)
         this_block = get_block(n,bid)
         do j=this_block%jb,this_block%je
         do i=this_block%ib,this_block%ie
            if (WORK(i,j,bid) > val) then
               val = WORK(i,j,bid)
               ival = this_block%i_glob(i)
               jval = this_block%j_glob(j)
            endif
         end do
         end do
      endif
   end do

!-----------------------------------------------------------------------
!EOC

 end subroutine global_maxloc_int

!***********************************************************************

 end module global_reductions

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
