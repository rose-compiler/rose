module m
  type t
    integer :: i
  end type t
contains
  subroutine s(x)
    implicit type(t)(x)
    dimension x(:)
!    print *, x(1)%i
  end subroutine s
end module m
