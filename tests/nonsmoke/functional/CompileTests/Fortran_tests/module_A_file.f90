! DQ (11/10/2008): Note module names use file name prefix to for use of 
! unique names and avoid race conditions in mod file generation.
module module_A_file_module_A

   implicit none
   private
   save

 ! This variable "r8" will be used a a kind parameter in the rest of the application
 !  integer, parameter, public :: r8 = selected_real_kind(13)

   integer x

end module module_A_file_module_A

