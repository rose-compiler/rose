! Bug report from Rice: 15-generic-typebound-procedure.f90
! A generic type-bound procedure name causes an assertion failure.

module m101
  type  :: t
  contains 
    generic :: p => q  ! assertion failure: name stack not empty
  end type
end module
