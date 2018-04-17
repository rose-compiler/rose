! Bug report from Rice: 16-empty-array-constructor-1.f90
! An array constructor with zero elements and only the empty string
! within its brackets causes the OFP parser to emit a syntax error message.

program p
  real :: a(0) = (/ /)  ! "no viable alternative"
! real :: b(0) = [ ]    ! ditto, although square brackets are F08 
end program
