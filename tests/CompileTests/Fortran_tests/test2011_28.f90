   implicit none
!  external :: test1, test2, ppTest
!  integer, pointer :: ptr1, ptr2(:)
!  integer, allocatable :: alloc2(:)
   procedure(), pointer :: pptr

   pptr => NULL()

end
