subroutine bar_f (i, j, k, x, f) bind(C, name = "bar")
  use iso_c_binding
  real (c_float) :: i
  integer (c_int) :: j
  integer         :: k
  integer (C_LONG) :: x
end subroutine
