
subroutine where_example()
  ! logical(logical_kind) :: should_set_cell(nx,ny,nz)
    logical(1) :: should_set_cell(5,6,7)
    logical(1) :: should_set_upwind(5,6,7), should_set_downwind(5,6,7), should_set_donor_volume(5,6,7)
    
  ! real(real_kind) :: donor_variability(nx,ny,nz), donor_mass(nx,ny,nz), donor_volume(nx,ny,nz)
    real(4) :: donor_variability(5,6,7), donor_mass(5,6,7), donor_volume(5,6,7)
    real(8) :: epsilon1(5,6,7), epsilon2(5,6,7), epsilon3(5,6,7), epsilon4(5,6,7)
    real(8) :: temperature1(5,6,7), temperature2(5,6,7), temperature3(5,6,7), temperature4(5,6,7)
    real(8) :: donor(5,6,7), upwind(5,6,7), downwind(5,6,7)

  ! DQ (8/21/2010): This will only compile with -rose:skip_syntax_check and -rose:skipfinalCompileStep (not supported by GNU)
  ! real(16) :: quad_donor(5,6,7), quad_upwind(5,6,7), quad_downwind(5,6,7)

end subroutine where_example
