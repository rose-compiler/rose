! Bug report from Rice: 20-derived-type-constructor-1.f03
! A derived type constructor in an initializer does not work. It causes the
! front end to fail an assertion.

program p
  type t
    integer :: c
  end type
  type(t) :: x = t(42)  ! assert false
end program
