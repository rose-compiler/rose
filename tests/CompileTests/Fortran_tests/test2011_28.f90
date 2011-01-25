! I think this is using F03 syntax for procedure pointers.
! So this might really be a FFortran 2003 test code 
! (syntax fails for gfortran, but is part of the gfortran test suite).

   implicit none
!  external :: test1, test2, ppTest
!  integer, pointer :: ptr1, ptr2(:)
!  integer, allocatable :: alloc2(:)
   procedure(), pointer :: pptr

   pptr => NULL()

end
