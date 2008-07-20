module module_A

   implicit none
!  private
!  save

 ! Error: "parameter" is being dropped from the unparsed code
 ! integer, parameter :: a0 = 4, a1 = a0 / 2
   integer, parameter :: b0 = 4, b1 = 7

end module module_A

