! Test declaration-type-spec, which is:
!      declaration-type-spec  is  intrinsic-type-spec (R403)
!                             or  TYPE ( derived-type-spec )
!                             or  CLASS ( derived-type-spec )
!                             or  CLASS ( * )
!
! Tested separately are: R403 and R455.
!
! declaration-type-spec is tested as part of a type-declaration-stmt (R501).
integer :: a          ! alt1
type(my_type) :: b    ! alt2
class(my_class) :: c  ! alt3
class(*) :: d         ! alt4

end

