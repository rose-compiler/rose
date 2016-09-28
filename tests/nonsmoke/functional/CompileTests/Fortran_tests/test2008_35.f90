module test2008_35_module_A

   implicit none
!  private
   save

   integer, parameter :: r8 = selected_real_kind(13)

end module test2008_35_module_A

module test2008_35_module_B

   use test2008_35_module_A

   implicit none
   private
   save

 ! This mean build a real type like r8 defined in the module: kinds_mod
   real (r8), parameter, public :: c0 = 0.0_r8

end module test2008_35_module_B
