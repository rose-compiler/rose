program foo

! Subroutines are not implemented properly in the parser (no c_actions being called)
! subroutine foo(i)
    integer :: i, j

    if (i < 1) then
       j = 1
    end if

!end subroutine
end
