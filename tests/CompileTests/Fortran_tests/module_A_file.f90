module module_A

   implicit none
   private
   save

 ! This variable "r8" will be used a a kind parameter in the rest of the application
   integer, parameter, public :: r8 = selected_real_kind(13)

end module module_A

