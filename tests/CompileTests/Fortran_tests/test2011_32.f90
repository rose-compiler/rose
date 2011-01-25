! { dg-do run }
! PR fortran/35830
!
subroutine f()
end subroutine

program test
  implicit none
  integer i
  abstract interface
    function ai()
      real, allocatable :: ai(:)
    end function
  end interface
  procedure(ai) :: f
!  if(any(f() /= [9,8,7])) call abort()
!  if(size(f()) /= 3) call abort()
end
