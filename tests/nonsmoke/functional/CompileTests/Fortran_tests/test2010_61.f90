! This testcode demonstrates case change issues that 
! were at least at some point a problem for ROSE or OFP.
subroutine Foo
end subroutine foo

program MAIN
  type T
    integer :: i
  end type t

  type(t) :: at

end program main

module mod61
end module MOD61

module MOD161
end module mod161

