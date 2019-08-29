subroutine bar_f (x) bind(C, name = "bar")
  use iso_c_binding
  integer (C_LONG_LONG) :: x
end subroutine
