 module broadcast

   implicit none
   private
   save

   public :: broadcast_scalar

   interface broadcast_scalar
   ! This is currently a bug to hanve a list larger than 1.
     module procedure broadcast_scalar_real,  &
                      broadcast_scalar_char
   end interface

 contains

   subroutine broadcast_scalar_real(scalar)
      real, intent(inout) :: scalar
   end subroutine broadcast_scalar_real

   subroutine broadcast_scalar_char(scalar)
      character, intent(inout) :: scalar
   end subroutine broadcast_scalar_char

end module broadcast

