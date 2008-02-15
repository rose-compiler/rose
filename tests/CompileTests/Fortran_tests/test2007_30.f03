subroutine foo(i)

integer :: i

! Works in OFP, and gfortran 4.1.2, but fails in gfortran 4.2.2
value :: i

end subroutine
