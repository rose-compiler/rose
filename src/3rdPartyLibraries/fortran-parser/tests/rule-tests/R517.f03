! Test intent-spec, which is:
!      intent-spec  is  IN
!                   or  OUT
!                   or  INOUT
!
! Tested as part of attr-spec (R503), which is part of a 
! type-declaration-stmt (R501).
integer, intent(in) :: a
integer, intent(out) :: b
integer, intent(inout) :: c
integer, intent(in out) :: d

end

