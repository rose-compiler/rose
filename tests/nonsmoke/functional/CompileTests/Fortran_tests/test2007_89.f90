subroutine example
    logical :: a
    real    :: b

  ! where statement with a string label accepted by gfortran, but not OFP (see test2007_88.f90)
  ! MY_WHERE: where (a) b = 0
  ! where statement with a numeric label
 10 if (a) then
       b = 0
    end if

end subroutine example
