! kind_selector can have an expr.  there are too many to test all of them, 
! but we can test some common ones.
! Testing kind selector.
integer (kind = 4), parameter :: i
integer (kind = 4) :: p
integer (kind = foo) :: bar
integer(8) :: j
integer(kind(4)) :: k
integer(kind=i) :: m
integer(kind(j)) :: n
integer(i) :: o
integer(selected_int_kind(12)) :: d
real * 7 :: k
end
