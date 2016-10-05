! The following bug was uncovered while debugging POP's forcing_shf.F90.
! The output rmod file incorrectly clobbers the type of the global variable zw.
! and makes it the same as that of the local variable zw in the subroutine.
! In addition, the local variable is incorrectly initialized to the same value
! as that of the global variable.
! Renaming either one of the variable hides the symptom.

module scopeBug2

   real  ::  zw = sqrt(0.001)

contains

subroutine calc_tpoints()
   integer,parameter :: zw = 5
end subroutine calc_tpoints

 end module
 
