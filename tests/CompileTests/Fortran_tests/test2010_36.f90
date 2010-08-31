! Test code extracted from POP code module file.
! This tests specification use of a type that has not be declarted yet.
module io_types

! !USES:

   implicit none
   private
   save

! !PUBLIC TYPES:

   ! Generic IO field descriptor

   integer, parameter, public :: i4 = selected_int_kind(6)

   type, public :: io_field_desc
!     integer(i4) :: id
!     type (io_dim), dimension(:), pointer :: field_dim
      type (io_dim), pointer :: field_dim
   end type

   type, public :: io_dim
!     integer(i4) :: id
      integer(i4) :: length  ! 1 to n, but 0 means unlimited
   end type

 contains

end module io_types
