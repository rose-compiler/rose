!interface test
!   subroutine foo()
!      integer :: i
!   end subroutine foo
!end interface test

! DQ (11/10/2008): Note module names use file name prefix to for use of 
! unique names and avoid race conditions in mod file generation.
module test2007_40_test
interface A
  subroutine foo()
    integer :: i 
  end subroutine foo
end interface A
end module test2007_40_test
