! S12-empty-string-constant.f90
! ROSE's unparser fails an assertion on an empty character string constant.
! It doesn't matter whether single or double quotes are used.

program p
 ! This is not legal Fortran "Error: Entity with assumed character length at (1) must be a dummy argument or a PARAMETER"
 ! character(*) :: c1 = ""  ! produces assertion failure in testTranslator
 ! character(*) :: c2 = ''  ! ditto

 ! Slightly modified test (still test the relevant problem).
   character(1) :: c1 = ""  ! produces assertion failure in testTranslator
   character(1) :: c2 = ''  ! ditto
end program
