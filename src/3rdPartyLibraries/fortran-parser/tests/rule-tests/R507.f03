! Test null-init, which is:
!      null-init  is  function-reference
!
! Function reference must be a reference to the NULL intrinsic function with
! no arguments (C506).
!
! null-init is tested as part of an initialization (R506), which is part of 
! an entity-decl (R504), which is part of a type-declaration-stmt (R501).
integer, pointer :: a => NULL()

end

