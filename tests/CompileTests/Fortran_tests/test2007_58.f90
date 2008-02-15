subroutine where_example
    real :: a(7), b(7), c(7)

  ! Need to discuss how to handle implicit functions
  ! upwind = sign(donor_variability,downwind) * min(abs(upwind), abs(downwind), epsilon3*abs(upwind) + epsilon4*abs(downwind))
    a = sign(b,c)

end subroutine where_example

