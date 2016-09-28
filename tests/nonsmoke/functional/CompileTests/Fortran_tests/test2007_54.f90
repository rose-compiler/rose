subroutine where_example
  ! logical(logical_kind) :: should_set_upwind(nx,ny,nz)    
  ! real(real_kind) :: donor(nx,ny,nz), upwind(nx,ny,nz), downwind(nx,ny,nz)
    logical(2) :: should_set_upwind(5,6,7)    
    real(4) :: donor(5,6,7), upwind(5,6,7), downwind(5,6,7)

    upwind = 0;
    where(should_set_upwind)
        upwind = donor - upwind
      ! upwind = 0;
    end where

end subroutine where_example

