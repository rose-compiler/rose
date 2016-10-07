subroutine example
    logical :: a(9)
    real    :: b(9)

  ! where statement with a string label accepted by gfortran, but not OFP
    MY_WHERE: where (a) b = 0

end subroutine example
