!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 module broadcast

!BOP
! !MODULE: broadcast
! !DESCRIPTION:
!  This module contains all the broadcast routines.  This
!  particular version contains serial versions of these routines
!  which typically perform no operations since there is no need
!  to broadcast what is already known.
!
! !REVISION HISTORY:
!  CVS:$Id: broadcast.F90,v 1.3 2004/01/07 19:56:38 pwjones Exp $
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
!  for serial codes, nothing is required
!
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
!  for serial codes, nothing is required
!
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
!  for serial codes, nothing is required
!
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
!  for serial codes, nothing is required
!
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
!  for serial codes, nothing is required
!
!-----------------------------------------------------------------------
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
!  for serial codes, nothing is required
!
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
!  for serial codes, nothing is required
!
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
!  for serial codes, nothing is required
!
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
!  for serial codes, nothing is required
!
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
!  for serial codes, nothing is required
!
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
!  for serial codes, nothing is required
!
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
!  for serial codes, nothing is required
!
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
!  for serial codes, nothing is required
!
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
!  for serial codes, nothing is required
!
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
!  for serial codes, nothing is required
!
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
!  for serial codes, nothing is required
!
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
!  for serial codes, nothing is required
!
!-----------------------------------------------------------------------
!EOC

 end subroutine broadcast_array_log_3d

!***********************************************************************

 end module broadcast

!|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
