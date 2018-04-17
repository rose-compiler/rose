! Bug report from Rice: 21-derived-type-constructor-2.txt
! A derived type constructor in an assignment statement does not work.
! It causes the front end to emit several error messages.

program p
  type t
    integer :: c
  end type
  type(t) :: x
  x = t(42)  ! ERROR: returning NULL from...
end program
