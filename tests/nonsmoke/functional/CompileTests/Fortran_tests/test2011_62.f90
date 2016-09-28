! From Craig: bug-3152988.f90
module test_struct

   type var_wrap
      real, dimension(:,:), pointer :: ptr2d
      integer :: sizes(3)
   end type

contains

subroutine foo()
   type(var_wrap) :: vw

   vw%ptr2d(1,vw%sizes(2)) = 1

end subroutine

end module
