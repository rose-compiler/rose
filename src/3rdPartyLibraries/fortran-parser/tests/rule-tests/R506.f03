! Test initialization, which is:
!      initialization  is  = initialization-expr
!                      or  => null-init
!
! Tested separately are: R730 (initialization-expr) and R507 (null-init).
!
! initialization is tested as part of an entity-decl (R504) that is part of
! a type-declaration-stmt (R501).
integer :: a = 1
integer, pointer :: b => NULL()

end

