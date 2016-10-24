subroutine where_example
    real :: a(7), b(7), c(7)

  ! Need to discuss how to handle implicit functions
    upwind = sign(donor_variability,downwind) * min(abs(upwind), abs(downwind), epsilon3*abs(upwind) + epsilon4*abs(downwind))
  !  a = sign(b,c) + min(abs(b), abs(c))
  ! a = abs(b) + abs(c)
  ! a = abs(b) + int(real(c),nint(c))
  ! a = sign(b,c) + int(real(c) + nint(c))
  ! a = sign(b,c) + int(real(c),nint(c))
    a = sign(b,c) + int(real(c))
    a = sign(b,c) + min(abs(b), abs(c))

end subroutine where_example

