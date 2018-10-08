!! R403 declaration-type-spec
!    is  intrinsic-type-spec (R404)
!    or  TYPE ( intrinsic-type-spec )
!    or  TYPE ( derived-type-spec )
!    or  CLASS ( derived-type-spec )
!    or  CLASS ( * )
!    or  TYPE  ( * )
!
! Tested separately are: R404 and R457.
!
! declaration-type-spec is tested as part of a type-declaration-stmt (R501).
integer :: a          ! alt1
type(REAL) :: b       ! alt2
type(my_type) :: c    ! alt3
CLASS(my_class) :: d  ! alt4
class ( * ) :: e      ! alt5
TYPE(*) :: f          ! alt6

end

