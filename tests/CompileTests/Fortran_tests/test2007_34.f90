subroutine foo()

! "i" is given a shape in the declaration, but "j" is not
integer :: i(:),j

! In general it appears that allocatable statements are discouraged in favor of using the allocatable attribute.
! "i" is declared to be allocatable so it's size is not known but it's shape was defined in its declaration
! "j" is given a shape in this allocatable statement and its size is deferred (until allocation).
allocatable :: i,j(:)

end subroutine


