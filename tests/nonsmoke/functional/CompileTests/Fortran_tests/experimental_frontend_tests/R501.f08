!! R501 type-declaration-stmt
!    is declaration-type-spec [ [ , attr-spec ] ... :: ] entity-decl-list

! intrinsic-type-spec, no attrs, w/ double colon
integer :: x
integer y
integer, pointer :: xptr
integer, pointer, allocatable :: yptr

! test entity_decl_list
real :: a,b

! intrinsic-type-spec with kind-selector
integer(kind=4) :: m

! intrinsic-type-spec with char-selector
10 Character(kind=1, len=10) :: string

! From NOTE 5.1

REAL A (10)
LOGICAL, DIMENSION (5, 5) :: MASK1, MASK2
COMPLEX :: CUBE_ROOT = (-0.5, 0.866)
INTEGER, PARAMETER :: SHORT = SELECTED_INT_KIND (4)
INTEGER (SHORT) K     !  Range at least -9999 to 9999.
REAL (KIND (0.0D0)) A
REAL (KIND = 2) B
COMPLEX (KIND = KIND (0.0D0)) :: C
CHARACTER (LEN = 10, KIND = 2)  A
CHARACTER B, C *20
TYPE (PERSON) :: CHAIRMAN
TYPE(NODE), POINTER :: HEAD => NULL ( )
TYPE (humongous_matrix (k=8, d=1000)) :: mat

end
