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

module mod
end module MOD

module MOD1
end module mod1

