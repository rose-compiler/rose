module module_A

   implicit none
!  private
   save

   integer, parameter :: r8 = selected_real_kind(13)

end module module_A

module module_B

   use module_A

   implicit none
   private
   save

 ! This mean build a real type like r8 defined in the module: kinds_mod
   real (r8), parameter, public :: c0 = 0.0_r8

end module module_B
