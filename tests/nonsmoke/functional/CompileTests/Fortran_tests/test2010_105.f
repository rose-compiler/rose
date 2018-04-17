! Bug report from Rice: 19-f77-scale.f
! The identifier 'scale' has no predefined meaning in Fortran 77.
! ROSE always treats it as the F95 intrinsic function named 'scale'.

      scale = 0.0
      end
