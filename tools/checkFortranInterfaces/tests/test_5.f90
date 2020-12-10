subroutine bar_f (i, j, x, f) bind(C, name = "bar")
  use iso_c_binding
  real (c_float) :: i
  integer (c_int) :: j
  integer (C_LONG) :: x
  character :: f
end subroutine
