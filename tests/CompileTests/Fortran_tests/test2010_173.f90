  ! real(real_kind) :: donor(nx,ny,nz), upwind(nx,ny,nz), downwind(nx,ny,nz)
  ! integer :: donor(5,6,7), upwind(5,6,7), downwind(5,6,7)
  ! integer :: donor(5,6,7) ! , upwind(5,6,7)
  ! real(4)    :: donor_variability(5,6,7)
    real(4)    :: result(5,6,7)
    real(4)    :: upwind(5,6,7)
    real(4)    :: downwind(5,6,7)

  ! upwind = donor - upwind
  ! upwind = donor

  ! upwind = sign(donor_variability,downwind) * min(abs(upwind), abs(downwind), epsilon3*abs(upwind) + epsilon4*abs(downwind))
  ! upwind = sign(upwind,downwind) * min(abs(upwind),abs(downwind),abs(upwind)+abs(downwind))
  ! upwind = min(abs(upwind),abs(downwind))
  ! upwind = min(upwind,downwind)
    result = min(abs(upwind),downwind)
end

