! Rice test
! Assertion failure
!
 subroutine g(*)
 end subroutine

program alt_return_call
call g(*123)  
123 continue
goto 123
end program
