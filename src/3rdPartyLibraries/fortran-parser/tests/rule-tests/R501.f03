! R501, intrinsic_type_spec, no attrs, w/ double colon
integer :: x
integer y
integer, pointer :: xptr
integer, pointer, allocatable :: yptr

! R501, test entity_decl_list
real :: a,b

! R501, intrinsic_type_spec with kind_selector
! kind_selector is R404.
integer(kind=4) :: m

! R501, intrinsic_type_spec with char_selector
! char_selector is R424.
character(kind=1, len=10) :: string

end
