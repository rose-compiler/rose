! Test code extracted from POP code module file.
! This tests the use of type-attr-spec pointer and also the case of "dimension(:)" and "pointer" combined together.
module io_types

! !USES:

   implicit none
   private
   save

! !PUBLIC TYPES:

   ! Generic IO field descriptor
   integer, public :: construct_file

   integer, parameter, public :: i4 = selected_int_kind(6)

   type, public :: io_dim
      integer(i4) :: id
      integer(i4) :: length  ! 1 to n, but 0 means unlimited
   end type

   type, public :: io_field_desc
      integer(i4) :: id
      type (io_dim), dimension(:), pointer :: field_dim
!     type (io_dim), pointer :: field_dim
   end type

 contains

! GNU 4.3 appears to require a function to be defined after a "contains" statement.
 function construct_file(id) result (descriptor)
      integer(i4) :: id
      integer :: descriptor  ! data file descriptor
 end function construct_file

end module io_types
