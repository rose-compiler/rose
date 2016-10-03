! Bug report from Rice: 17-empty-array-constructor-2.f90
! An array constructor with zero elements and a type-spec within its
! brackets causes the front end to fail an assertion.

program p
  real :: a(0) = (/ real :: /)  ! causes an 'Assert(false)'
! real :: b(0) = [ real :: ]    ! ditto, although square brackets are F08
end program
