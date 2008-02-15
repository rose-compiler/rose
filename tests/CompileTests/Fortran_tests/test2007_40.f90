!interface test
!   subroutine foo()
!      integer :: i
!   end subroutine foo
!end interface test

module test
interface A
  subroutine foo()
    integer :: i 
  end subroutine foo
end interface A
end module test
