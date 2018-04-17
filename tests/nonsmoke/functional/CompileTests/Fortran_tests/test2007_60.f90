subroutine where_example
    real :: a(7), b(7)

  ! temperature4 = global_eoshift(donor_variability, SHIFT=1, BOUNDARY=zero, DIM=id)
  ! a = eoshift(b, SHIFT=1, BOUNDARY=0, DIM=1)
    a = eoshift(b, SHIFT=1)

end subroutine where_example


