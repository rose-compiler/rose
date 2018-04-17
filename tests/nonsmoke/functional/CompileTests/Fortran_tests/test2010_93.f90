! Bug report from Rice: 07-enum-endconstruct-assert.f90
! The enum construct produces an assertion failure saying that the enum's
! start- and end-of-construct lines are not different.

subroutine g
  enum, bind(c)
    enumerator e
  end enum
end subroutine
