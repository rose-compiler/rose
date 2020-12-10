subroutine bar_f (j, x, f) bind(C, name = "bar")
  use iso_c_binding
  integer (c_int) :: j
  integer (C_LONG) :: x
  integer (c_short) :: f
end subroutine
