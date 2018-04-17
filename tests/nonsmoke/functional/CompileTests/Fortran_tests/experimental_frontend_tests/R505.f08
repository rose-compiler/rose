!! R505 initialization
!           is  = initialization-expr
!           or  => null-init
!           or => initial-data-target
!
! Tested separately are: R730 (initialization-expr) and R507 (null-init).
!
! initialization is tested as part of an entity-decl (R503) that is part of
! a type-declaration-stmt (R501).

integer :: a = 1
integer, pointer :: b => NULL()
integer, pointer :: aptr => a   !initial-data-target

end

