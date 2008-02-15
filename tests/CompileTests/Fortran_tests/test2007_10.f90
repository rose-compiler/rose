program foo

! Subroutines are not implemented properly in the parser (no c_actions being called)
! subroutine foo(i)
    integer :: i, j

 10 if (i < 1) then
        j = 0
 20  else
        i = 4
 30 end if

! end subroutine
end program
