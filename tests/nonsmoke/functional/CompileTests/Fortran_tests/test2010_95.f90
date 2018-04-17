! Bug report from Rice: 09-class-not-implemented.f90
! The keyword 'class' is not implemented as a declaration type specifier.

program p

  type, abstract :: a
  end type

  type t
    integer:: k
    class(a), pointer :: p  ! "Sorry, not implemented"
  end type

end program
