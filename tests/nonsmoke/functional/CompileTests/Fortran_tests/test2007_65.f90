subroutine where_example
!   logical(logical_kind) :: should_set_cell(nx,ny,nz)
!   logical(logical_kind) :: should_set_upwind(nx,ny,nz), should_set_downwind(nx,ny,nz), should_set_donor_volume(nx,ny,nz)
    
!   real(real_kind) :: donor_variability(nx,ny,nz), donor_mass(nx,ny,nz), donor_volume(nx,ny,nz)
!   real(real_kind) :: epsilon1(nx,ny,nz), epsilon2(nx,ny,nz), epsilon3(nx,ny,nz), epsilon4(nx,ny,nz)
!   real(real_kind) :: temperature1(nx,ny,nz), temperature2(nx,ny,nz), temperature3(nx,ny,nz), temperature4(nx,ny,nz)
!   real(real_kind) :: donor(nx,ny,nz), upwind(nx,ny,nz), downwind(nx,ny,nz)
    real(4) :: donor(5,6,7), upwind(5,6,7), downwind(5,6,7)

    upwind = donor
    
  !  where(should_set_upwind)
  !     upwind = donor - upwind
  !     where (upwind < zero_epsilon) upwind = zero
  !  elsewhere
  !      upwind = zero
  !  end where
    
  !  where(should_set_downwind)
  !      downwind = downwind - donor
  !      where (downwind < zero_epsilon) downwind = zero
  !  elsewhere
  !      downwind = zero
  !  end where
    
  ! donor_variability = zero
  !  where (upwind*downwind > zero) donor_variability = epsilon2
    
  !  upwind = sign(donor_variability,downwind) * &
  !      min(abs(upwind), abs(downwind), epsilon3*abs(upwind) + epsilon4*abs(downwind))
    
  !  donor_variability = donor_mass*(donor + upwind)

   ! temperature4 = global_eoshift(donor_variability, SHIFT=1, BOUNDARY=zero, DIM=id)

   ! where(should_set_cell) result(:,:,:,l) = temperature2*(result(:,:,:,l)*temperature1 + (donor_variability - temperature4))
end subroutine where_example


