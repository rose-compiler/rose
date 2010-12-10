! The following bug was uncovered while debugging forcing_shf.F90.
! The output rmod file incorrectly clobbers the type of the global variable zw.
! and makes it the same type as the local variable zw in the subroutine.
! See output rmod file appended to the source code below
!
module scopeBug1

   real, parameter :: x = 3.14
   real  ::  zw = sqrt(x)

contains

subroutine calc_tpoints()
   integer :: x
end subroutine calc_tpoints

end module
