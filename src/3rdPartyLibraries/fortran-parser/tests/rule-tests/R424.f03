! Test char_selector (R424)
! char_selector alt1, char_length alt2, no optional comma
character*5 :: a
! char_selector alt1, char_length alt1, no optional comma
character*(*) :: b
! char_selector alt1, char_length alt1, no optional comma
character*(:) :: c
! char_selector alt1, char_length alt1, no optional comma
character*(foo(a)) :: d
! char_selector alt1, char_length alt2, optional comma
character*5, e
! char_selector alt1, char_length alt1, optional comma
character*(*), f
! char_selector alt1, char_length alt1, optional comma
character*(:) :: g
! char_selector alt1, char_length alt1, optional comma
character*(foo(a)) :: h

! the following tests were created by looking at the standard rather than
! the rules in FortranParser.g

! char_selector => ( LEN = type-param-value, 
!                    KIND = scalar-int-initialization-expr)
character(len=*, kind=1) :: i
character(len=:, kind=1) :: j
character(len=foo(2), kind=1) :: k

! char_selector => ( type-param-value, 
!                    [ KIND = ] scalar-int-initialization-expr )
character(*, kind=1) :: l
character(*, 1) :: m
character(:, kind=1) :: n
character(:, 1) :: o
character(foo(2), kind=1) :: p
character(foo(2), 1) :: q

! char_selector => ( KIND = scalar-int-initialization-expr
!                    [, LEN=type-param-value])
character(kind=1) :: r
character(kind=1, len=*) :: s
character(kind=1, len=:) :: t
character(kind=1, len=foo(2)) :: u

! char_selector => length-selector (R425)
character(len=*) :: v
character(len=:) :: w
character(len=foo(2)) :: x
character(*) :: y
character(:) :: z
character(foo(2)) :: aa
! the * char-length [,] cases are tested at the top.

end

