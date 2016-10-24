subroutine where_example
  ! real(real_kind) :: donor(nx,ny,nz), upwind(nx,ny,nz), downwind(nx,ny,nz)
  ! integer :: donor(5,6,7), upwind(5,6,7), downwind(5,6,7)
    integer :: donor(5,6,7), upwind(5,6,7)

  ! upwind = donor - upwind
    upwind = donor

end subroutine where_example

