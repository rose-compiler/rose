! DQ (11/10/2008): Note module names use file name prefix to for use of 
! unique names and avoid race conditions in mod file generation.
module module_B_file_module_B

   use module_A_file_module_A

   implicit none
   private
   save

 ! This means build a real type like "r8" defined in module "module_A"
 ! Note: it is a current bug that the initializer is unparsed as "0.0" instead of "0.0_r8"
 !  real (r8), parameter, public :: c0 = 0.0_r8

  integer y

end module module_B_file_module_B


