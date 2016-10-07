! I think this is using F03 syntax for procedure pointers.
! So this might really be a FFortran 2003 test code 
! (syntax fails for gfortran, but is part of the gfortran test suite).

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
  if(any(f() /= [9,8,7])) call abort()
  if(size(f()) /= 3) call abort()
end
