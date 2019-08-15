module test_derived_types

  use iso_c_binding

  contains

  subroutine fu (i, x) bind(C, name = "fubar")
    integer(c_int) :: i(100)
    real(c_float) :: x(20)
  end subroutine fu
  
end module
