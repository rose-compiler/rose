! Rice test
! Initializing a variable of derived type with a constructor causes the front end to fail an assertion:
! FortranParserActionROSE.C:3554: ... Assertion `false' failed.

module constructor_init

  type t
    integer c
  end type

  type(t) :: x = t(1)

end module
