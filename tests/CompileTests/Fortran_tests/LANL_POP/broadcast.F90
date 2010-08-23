!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module broadcast

!BOP
! !MODULE: broadcast
! !DESCRIPTION:
!  This module contains all the broadcast routines.  This
!  particular version contains MPI versions of these routines.
!
! !REVISION HISTORY:
!  CVS:$Id: broadcast.F90,v 1.2 2004/01/07 19:56:32 pwjones Exp $
!  CVS:$Name: POP_2_0_1 $
!
! !USES:

   use kinds_mod
   use communicate

   implicit none
   private
   save

! !PUBLIC MEMBER FUNCTIONS:

   public  :: broadcast_scalar,         &
              broadcast_array

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  generic interfaces for module procedures
!
!-----------------------------------------------------------------------

   interface broadcast_scalar
     module procedure broadcast_scalar_dbl,  &
                      broadcast_scalar_real, &
                      broadcast_scalar_int,  &
                      broadcast_scalar_log,  &
                      broadcast_scalar_char
   end interface

   interface broadcast_array
     module procedure broadcast_array_dbl_1d,  &
                      broadcast_array_real_1d, &
                      broadcast_array_int_1d,  &
                      broadcast_array_log_1d,  &
                      broadcast_array_dbl_2d,  &
                      broadcast_array_real_2d, &
                      broadcast_array_int_2d,  &
                      broadcast_array_log_2d,  &
                      broadcast_array_dbl_3d,  &
                      broadcast_array_real_3d, &
                      broadcast_array_int_3d,  &
                      broadcast_array_log_3d
   end interface

!EOC
!***********************************************************************

 contains

!***********************************************************************
!BOP
! !IROUTINE: broadcast_scalar_dbl
! !INTERFACE:

 subroutine broadcast_scalar_dbl(scalar, root_pe)

! !DESCRIPTION:
!  Broadcasts a scalar dbl variable from one processor (root_pe)
!  to all other processors. This is a specific instance of the generic
!  broadcast\_scalar interface.
!
! !REVISION HISTORY:
!  same as module

! !INCLUDES:

   include 'mpif.h'  ! MPI Fortran include file

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
      root_pe              ! processor number to broadcast from

! !INPUT/OUTPUT PARAMETERS:

   real (r8), intent(inout) :: &
      scalar               ! scalar to be broadcast

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: ierr  ! local MPI error flag

!-----------------------------------------------------------------------

   call MPI_BCAST(scalar, 1, MPI_DBL, root_pe, MPI_COMM_OCN, ierr)
   call MPI_BARRIER(MPI_COMM_OCN, ierr)

!-----------------------------------------------------------------------
!EOC

end subroutine broadcast_scalar_dbl

!***********************************************************************
!BOP
! !IROUTINE: broadcast_scalar_real
! !INTERFACE:

subroutine broadcast_scalar_real(scalar, root_pe)

! !DESCRIPTION:
!  Broadcasts a scalar real variable from one processor (root_pe)
!  to all other processors. This is a specific instance of the generic
!  broadcast\_scalar interface.
!
! !REVISION HISTORY:
!  same as module
!
! !INCLUDES:

   include 'mpif.h'  ! MPI Fortran include file

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
      root_pe              ! processor number to broadcast from

! !INPUT/OUTPUT PARAMETERS:

   real (r4), intent(inout) :: &
      scalar               ! scalar to be broadcast

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: ierr  ! local MPI error flag

!-----------------------------------------------------------------------

   call MPI_BCAST(scalar, 1, MPI_REAL, root_pe, MPI_COMM_OCN, ierr)
   call MPI_BARRIER(MPI_COMM_OCN, ierr)

!-----------------------------------------------------------------------
!EOC

 end subroutine broadcast_scalar_real

!***********************************************************************
!BOP
! !IROUTINE: broadcast_scalar_int
! !INTERFACE:

subroutine broadcast_scalar_int(scalar, root_pe)

! !DESCRIPTION:
!  Broadcasts a scalar integer variable from one processor (root_pe)
!  to all other processors. This is a specific instance of the generic
!  broadcast\_scalar interface.
!
! !REVISION HISTORY:
!  same as module

! !INCLUDES:

   include 'mpif.h'  ! MPI Fortran include file

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
      root_pe              ! processor number to broadcast from

! !INPUT/OUTPUT PARAMETERS:

   integer (int_kind), intent(inout) :: &
      scalar                ! scalar to be broadcast

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: ierr  ! local MPI error flag

!-----------------------------------------------------------------------

   call MPI_BCAST(scalar, 1, MPI_INTEGER, root_pe, MPI_COMM_OCN,ierr)
   call MPI_BARRIER(MPI_COMM_OCN, ierr)

!-----------------------------------------------------------------------
!EOC

 end subroutine broadcast_scalar_int

!***********************************************************************
!BOP
! !IROUTINE: broadcast_scalar_log
! !INTERFACE:

subroutine broadcast_scalar_log(scalar, root_pe)

! !DESCRIPTION:
!  Broadcasts a scalar logical variable from one processor (root_pe)
!  to all other processors. This is a specific instance of the generic
!  broadcast\_scalar interface.
!
! !REVISION HISTORY:
!  same as module

! !INCLUDES:

   include 'mpif.h'  ! MPI Fortran include file

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
     root_pe              ! processor number to broadcast from

! !INPUT/OUTPUT PARAMETERS:

   logical (log_kind), intent(inout) :: &
     scalar               ! scalar to be broadcast

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
     itmp,               &! local temporary
     ierr                 ! MPI error flag

!-----------------------------------------------------------------------

   if (scalar) then
     itmp = 1
   else
     itmp = 0
   endif

   call MPI_BCAST(itmp, 1, MPI_INTEGER, root_pe, MPI_COMM_OCN, ierr)
   call MPI_BARRIER(MPI_COMM_OCN, ierr)

   if (itmp == 1) then
     scalar = .true.
   else
     scalar = .false.
   endif

!-----------------------------------------------------------------------
!EOC

 end subroutine broadcast_scalar_log

!***********************************************************************
!BOP
! !IROUTINE: broadcast_scalar_char
! !INTERFACE:

subroutine broadcast_scalar_char(scalar, root_pe)

! !DESCRIPTION:
!  Broadcasts a scalar character variable from one processor (root_pe)
!  to all other processors. This is a specific instance of the generic
!  broadcast\_scalar interface.
!
! !REVISION HISTORY:
!  same as module

! !INCLUDES:

   include 'mpif.h'  ! MPI Fortran include file

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
     root_pe              ! processor number to broadcast from

! !INPUT/OUTPUT PARAMETERS:

   character (*), intent(inout) :: &
     scalar               ! scalar to be broadcast

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
     clength,            &! length of character
     ierr                 ! MPI error flag

!-----------------------------------------------------------------------

   clength = len(scalar)

   call MPI_BCAST(scalar, clength, MPI_CHARACTER, root_pe, MPI_COMM_OCN, ierr)
   call MPI_BARRIER(MPI_COMM_OCN, ierr)

!--------------------------------------------------------------------
!EOC

 end subroutine broadcast_scalar_char

!***********************************************************************
!BOP
! !IROUTINE: broadcast_array_dbl_1d
! !INTERFACE:

subroutine broadcast_array_dbl_1d(array, root_pe)

! !DESCRIPTION:
!  Broadcasts a vector dbl variable from one processor (root_pe)
!  to all other processors. This is a specific instance of the generic
!  broadcast\_array interface.
!
! !REVISION HISTORY:
!  same as module

! !INCLUDES:

   include 'mpif.h'  ! MPI Fortran include file

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
     root_pe           ! processor number to broadcast from

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(:), intent(inout) :: &
     array             ! array to be broadcast

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
     nelements,       &! size of array
     ierr              ! local MPI error flag

!-----------------------------------------------------------------------

   nelements = size(array)

   call MPI_BCAST(array, nelements, MPI_DBL, root_pe, MPI_COMM_OCN, ierr)
   call MPI_BARRIER(MPI_COMM_OCN, ierr)

!-----------------------------------------------------------------------
!EOC

 end subroutine broadcast_array_dbl_1d

!***********************************************************************
!BOP
! !IROUTINE: broadcast_array_real_1d
! !INTERFACE:

subroutine broadcast_array_real_1d(array, root_pe)

! !DESCRIPTION:
!  Broadcasts a real vector from one processor (root_pe)
!  to all other processors. This is a specific instance of the generic
!  broadcast\_array interface.
!
! !REVISION HISTORY:
!  same as module

! !INCLUDES:

   include 'mpif.h'  ! MPI Fortran include file

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
     root_pe              ! processor number to broadcast from

! !INPUT/OUTPUT PARAMETERS:

   real (r4), dimension(:), intent(inout) :: &
     array                ! array to be broadcast

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
     nelements,          &! size of array to be broadcast
     ierr                 ! local MPI error flag

!-----------------------------------------------------------------------

   nelements = size(array)

   call MPI_BCAST(array, nelements, MPI_REAL, root_pe, MPI_COMM_OCN, ierr)
   call MPI_BARRIER(MPI_COMM_OCN, ierr)

!-----------------------------------------------------------------------
!EOC

 end subroutine broadcast_array_real_1d

!***********************************************************************
!BOP
! !IROUTINE: broadcast_array_int_1d
! !INTERFACE:

subroutine broadcast_array_int_1d(array, root_pe)

! !DESCRIPTION:
!  Broadcasts an integer vector from one processor (root_pe)
!  to all other processors. This is a specific instance of the generic
!  broadcast\_array interface.
!
! !REVISION HISTORY:
!  same as module

! !INCLUDES:

   include 'mpif.h'  ! MPI Fortran include file

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
     root_pe              ! processor number to broadcast from

! !INPUT/OUTPUT PARAMETERS:

   integer (int_kind), dimension(:), intent(inout) :: &
       array              ! array to be broadcast

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
     nelements,          &! size of array to be broadcast
     ierr                 ! local MPI error flag

!-----------------------------------------------------------------------

   nelements = size(array)

   call MPI_BCAST(array, nelements, MPI_INTEGER, root_pe, MPI_COMM_OCN, ierr)
   call MPI_BARRIER(MPI_COMM_OCN, ierr)

!-----------------------------------------------------------------------
!EOC

 end subroutine broadcast_array_int_1d

!***********************************************************************
!BOP
! !IROUTINE: broadcast_array_log_1d
! !INTERFACE:

subroutine broadcast_array_log_1d(array, root_pe)

! !DESCRIPTION:
!  Broadcasts a logical vector from one processor (root_pe)
!  to all other processors. This is a specific instance of the generic
!  broadcast\_array interface.
!
! !REVISION HISTORY:
!  same as module

! !INCLUDES:

   include 'mpif.h'  ! MPI Fortran include file

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
     root_pe              ! processor number to broadcast from

! !INPUT/OUTPUT PARAMETERS:

   logical (log_kind), dimension(:), intent(inout) :: &
     array                ! array to be broadcast

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind), dimension(:), allocatable :: &
      array_int            ! temporary array for MPI bcast

   integer (int_kind) :: &
      nelements,          &! size of array to be broadcast
      ierr                 ! local MPI error flag

!-----------------------------------------------------------------------

   nelements = size(array)
   allocate(array_int(nelements))

   where (array)
     array_int = 1
   elsewhere
     array_int = 0
   end where

   call MPI_BCAST(array_int, nelements, MPI_INTEGER, root_pe, &
                  MPI_COMM_OCN, ierr)
   call MPI_BARRIER(MPI_COMM_OCN, ierr)

   where (array_int == 1)
     array = .true.
   elsewhere
     array = .false.
   end where

   deallocate(array_int)

!-----------------------------------------------------------------------
!EOC

 end subroutine broadcast_array_log_1d

!***********************************************************************
!BOP
! !IROUTINE: broadcast_array_dbl_2d
! !INTERFACE:

 subroutine broadcast_array_dbl_2d(array, root_pe)

! !DESCRIPTION:
!  Broadcasts a dbl 2d array from one processor (root_pe)
!  to all other processors. This is a specific instance of the generic
!  broadcast\_array interface.
!
! !REVISION HISTORY:
!  same as module

! !INCLUDES:

   include 'mpif.h'  ! MPI Fortran include file

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
     root_pe           ! processor number to broadcast from

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(:,:), intent(inout) :: &
     array             ! array to be broadcast

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
      nelements,         &! size of array
      ierr                ! local MPI error flag

!-----------------------------------------------------------------------

   nelements = size(array)

   call MPI_BCAST(array, nelements, MPI_DBL, root_pe, MPI_COMM_OCN, ierr)
   call MPI_BARRIER(MPI_COMM_OCN, ierr)

!-----------------------------------------------------------------------
!EOC

 end subroutine broadcast_array_dbl_2d

!***********************************************************************
!BOP
! !IROUTINE: broadcast_array_real_2d
! !INTERFACE:

 subroutine broadcast_array_real_2d(array, root_pe)

! !DESCRIPTION:
!  Broadcasts a real 2d array from one processor (root_pe)
!  to all other processors. This is a specific instance of the generic
!  broadcast\_array interface.
!
! !REVISION HISTORY:
!  same as module

! !INCLUDES:

   include 'mpif.h'  ! MPI Fortran include file

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
     root_pe              ! processor number to broadcast from

! !INPUT/OUTPUT PARAMETERS:

   real (r4), dimension(:,:), intent(inout) :: &
     array                ! array to be broadcast

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
     nelements,          &! size of array to be broadcast
     ierr                 ! local MPI error flag

!-----------------------------------------------------------------------

   nelements = size(array)

   call MPI_BCAST(array, nelements, MPI_REAL, root_pe, MPI_COMM_OCN, ierr)
   call MPI_BARRIER(MPI_COMM_OCN, ierr)

!-----------------------------------------------------------------------
!EOC

 end subroutine broadcast_array_real_2d

!***********************************************************************
!BOP
! !IROUTINE: broadcast_array_int_2d
! !INTERFACE:

 subroutine broadcast_array_int_2d(array, root_pe)

! !DESCRIPTION:
!  Broadcasts a 2d integer array from one processor (root_pe)
!  to all other processors. This is a specific instance of the generic
!  broadcast\_array interface.
!
! !REVISION HISTORY:
!  same as module

! !INCLUDES:

   include 'mpif.h'  ! MPI Fortran include file

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
     root_pe              ! processor number to broadcast from

! !INPUT/OUTPUT PARAMETERS:

   integer (int_kind), dimension(:,:), intent(inout) :: &
       array              ! array to be broadcast

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
     nelements,          &! size of array to be broadcast
     ierr                 ! local MPI error flag

!-----------------------------------------------------------------------

   nelements = size(array)

   call MPI_BCAST(array, nelements, MPI_INTEGER, root_pe, MPI_COMM_OCN, ierr)
   call MPI_BARRIER(MPI_COMM_OCN, ierr)

!-----------------------------------------------------------------------
!EOC

 end subroutine broadcast_array_int_2d

!***********************************************************************
!BOP
! !IROUTINE: broadcast_array_log_2d
! !INTERFACE:

 subroutine broadcast_array_log_2d(array, root_pe)

! !DESCRIPTION:
!  Broadcasts a logical 2d array from one processor (root_pe)
!  to all other processors. This is a specific instance of the generic
!  broadcast\_array interface.
!
! !REVISION HISTORY:
!  same as module

! !INCLUDES:

   include 'mpif.h'  ! MPI Fortran include file

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
     root_pe              ! processor number to broadcast from

! !INPUT/OUTPUT PARAMETERS:

   logical (log_kind), dimension(:,:), intent(inout) :: &
     array                ! array to be broadcast

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind), dimension(:,:), allocatable :: &
     array_int            ! temporary array for MPI bcast

   integer (int_kind) :: &
     nelements,          &! size of array to be broadcast
     ierr                 ! local MPI error flag

!-----------------------------------------------------------------------

   nelements = size(array)
   allocate(array_int(size(array,dim=1),size(array,dim=2)))

   where (array)
     array_int = 1
   elsewhere
     array_int = 0
   end where

   call MPI_BCAST(array_int, nelements, MPI_INTEGER, root_pe, &
                  MPI_COMM_OCN, ierr)
   call MPI_BARRIER(MPI_COMM_OCN, ierr)

   where (array_int == 1)
     array = .true.
   elsewhere
     array = .false.
   end where

   deallocate(array_int)

!-----------------------------------------------------------------------
!EOC

 end subroutine broadcast_array_log_2d

!***********************************************************************
!BOP
! !IROUTINE: broadcast_array_dbl_3d
! !INTERFACE:

 subroutine broadcast_array_dbl_3d(array, root_pe)

! !DESCRIPTION:
!  Broadcasts a double 3d array from one processor (root_pe)
!  to all other processors. This is a specific instance of the generic
!  broadcast\_array interface.
!
! !REVISION HISTORY:
!  same as module

! !INCLUDES:

   include 'mpif.h'  ! MPI Fortran include file

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
     root_pe           ! processor number to broadcast from

! !INPUT/OUTPUT PARAMETERS:

   real (r8), dimension(:,:,:), intent(inout) :: &
     array             ! array to be broadcast

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
     nelements,       &! size of array
     ierr              ! local MPI error flag

!-----------------------------------------------------------------------

   nelements = size(array)

   call MPI_BCAST(array, nelements, MPI_DBL, root_pe, MPI_COMM_OCN, ierr)
   call MPI_BARRIER(MPI_COMM_OCN, ierr)

!-----------------------------------------------------------------------
!EOC

 end subroutine broadcast_array_dbl_3d

!***********************************************************************
!BOP
! !IROUTINE: broadcast_array_real_3d
! !INTERFACE:

 subroutine broadcast_array_real_3d(array, root_pe)

! !DESCRIPTION:
!  Broadcasts a real 3d array from one processor (root_pe)
!  to all other processors. This is a specific instance of the generic
!  broadcast\_array interface.
!
! !REVISION HISTORY:
!  same as module

! !INCLUDES:

   include 'mpif.h'  ! MPI Fortran include file

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
     root_pe              ! processor number to broadcast from

! !INPUT/OUTPUT PARAMETERS:

   real (r4), dimension(:,:,:), intent(inout) :: &
     array                ! array to be broadcast

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
     nelements,          &! size of array to be broadcast
     ierr                 ! local MPI error flag

!-----------------------------------------------------------------------

   nelements = size(array)

   call MPI_BCAST(array, nelements, MPI_REAL, root_pe, MPI_COMM_OCN, ierr)
   call MPI_BARRIER(MPI_COMM_OCN, ierr)

!-----------------------------------------------------------------------
!EOC

 end subroutine broadcast_array_real_3d

!***********************************************************************
!BOP
! !IROUTINE: broadcast_array_int_3d
! !INTERFACE:

 subroutine broadcast_array_int_3d(array, root_pe)

! !DESCRIPTION:
!  Broadcasts an integer 3d array from one processor (root_pe)
!  to all other processors. This is a specific instance of the generic
!  broadcast\_array interface.
!
! !REVISION HISTORY:
!  same as module

! !INCLUDES:

   include 'mpif.h'  ! MPI Fortran include file

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
     root_pe              ! processor number to broadcast from

! !INPUT/OUTPUT PARAMETERS:

   integer (int_kind), dimension(:,:,:), intent(inout) :: &
       array              ! array to be broadcast

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind) :: &
     nelements,          &! size of array to be broadcast
     ierr                 ! local MPI error flag

!-----------------------------------------------------------------------

   nelements = size(array)

   call MPI_BCAST(array, nelements, MPI_INTEGER, root_pe, MPI_COMM_OCN, ierr)
   call MPI_BARRIER(MPI_COMM_OCN, ierr)

!-----------------------------------------------------------------------
!EOC

 end subroutine broadcast_array_int_3d

!***********************************************************************
!BOP
! !IROUTINE: broadcast_array_log_3d
! !INTERFACE:

 subroutine broadcast_array_log_3d(array, root_pe)

! !DESCRIPTION:
!  Broadcasts a logical 3d array from one processor (root_pe)
!  to all other processors. This is a specific instance of the generic
!  broadcast\_array interface.
!
! !REVISION HISTORY:
!  same as module

! !INCLUDES:

   include 'mpif.h'  ! MPI Fortran include file

! !INPUT PARAMETERS:

   integer (int_kind), intent(in) :: &
     root_pe              ! processor number to broadcast from

! !INPUT/OUTPUT PARAMETERS:

   logical (log_kind), dimension(:,:,:), intent(inout) :: &
     array                ! array to be broadcast

!EOP
!BOC
!-----------------------------------------------------------------------
!
!  local variables
!
!-----------------------------------------------------------------------

   integer (int_kind), dimension(:,:,:), allocatable :: &
     array_int            ! temporary array for MPI bcast

   integer (int_kind) :: &
     nelements,          &! size of array to be broadcast
     ierr                 ! local MPI error flag

!-----------------------------------------------------------------------

   nelements = size(array)
   allocate(array_int(size(array,dim=1), &
                      size(array,dim=2), &
                      size(array,dim=3)))

   where (array)
     array_int = 1
   elsewhere
     array_int = 0
   end where

   call MPI_BCAST(array_int, nelements, MPI_INTEGER, root_pe, &
                  MPI_COMM_OCN, ierr)
   call MPI_BARRIER(MPI_COMM_OCN, ierr)

   where (array_int == 1)
     array = .true.
   elsewhere
     array = .false.
   end where

   deallocate(array_int)

!-----------------------------------------------------------------------
!EOC

 end subroutine broadcast_array_log_3d

!***********************************************************************

 end module broadcast

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
