subroutine foobar (i) bind(C, name = "fubar")
  integer, value :: i
end subroutine foobar

subroutine bar_f (ch, i, x, y) bind(C, name = "bar")
  integer, value :: ch
  integer :: i
  real, value :: x
  character, value :: y
end subroutine
