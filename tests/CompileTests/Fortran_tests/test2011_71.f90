! constructor-initializer.f90
! Initializing a variable of derived type with a constructor causes the front end to fail an assertion:
! FortranParserActionROSE.C:3554: ... Assertion `false' failed.

module m

  type t
    integer c
  end type

  type(t) :: x = t(1)

end module
