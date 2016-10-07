subroutine where_example
  ! These declarations do not pass the gfortran syntax checking
  ! logical(logical_kind) :: should_set_cell(nx,ny,nz)
  ! logical(logical_kind) :: should_set_upwind(nx,ny,nz), should_set_downwind(nx,ny,nz), should_set_donor_volume(nx,ny,nz)

  ! real(real_kind) :: donor_variability(nx,ny,nz), donor_mass(nx,ny,nz), donor_volume(nx,ny,nz)
  ! real(real_kind) :: epsilon1(nx,ny,nz), epsilon2(nx,ny,nz), epsilon3(nx,ny,nz), epsilon4(nx,ny,nz)
  ! real(real_kind) :: temperature1(nx,ny,nz), temperature2(nx,ny,nz), temperature3(nx,ny,nz), temperature4(nx,ny,nz)
  ! real(real_kind) :: donor(nx,ny,nz), upwind(nx,ny,nz), downwind(nx,ny,nz)

    logical(1) :: should_set_cell(5,6,7)
    logical(1) :: should_set_upwind(5,6,7), should_set_downwind(5,6,7), should_set_donor_volume(5,6,7)
    
    real(4) :: donor_variability(5,6,7), donor_mass(5,6,7), donor_volume(5,6,7)
    real(4) :: epsilon1(5,6,7), epsilon2(5,6,7), epsilon3(5,6,7), epsilon4(5,6,7)
    real(4) :: temperature1(5,6,7), temperature2(5,6,7), temperature3(5,6,7), temperature4(5,6,7)
    real(4) :: donor(5,6,7), upwind(5,6,7), downwind(5,6,7)
    
    where(should_set_upwind)
        upwind = donor - upwind
      ! where (upwind < zero_epsilon) upwind = zero
    elsewhere
        upwind = zero
    end where
    
    where(should_set_downwind)
        downwind = downwind - donor

      ! where statement not implemented in ROSE
      ! where (downwind < zero_epsilon) downwind = zero
    elsewhere
        downwind = zero
    end where
    
    donor_variability = zero
  ! where statement not implemented in ROSE
  ! where (upwind*downwind > zero) donor_variability = epsilon2
    
    upwind = sign(donor_variability,downwind) * &
        min(abs(upwind), abs(downwind), epsilon3*abs(upwind) + epsilon4*abs(downwind))
    
    donor_variability = donor_mass*(donor + upwind)
    
  ! temperature4 = global_eoshift(donor_variability, SHIFT=1, BOUNDARY=zero, DIM=id)
    temperature4 = eoshift(donor_variability, SHIFT=1, BOUNDARY=zero, DIM=id)

  ! where statement not implemented in ROSE
  ! where(should_set_cell) result(:,:,:,l) = temperature2*(result(:,:,:,l)*temperature1 + (donor_variability - temperature4))

  ! This does not pass the gfortran syntax checking: Error: Syntax error in WHERE statement
  ! result(:,:,:,l) = temperature2*(result(:,:,:,l)*temperature1 + (donor_variability - temperature4))
end subroutine where_example

