! This shows how only the c_actions that are specific to subroutines are not called.
program main
end program main

subroutine foo()
end subroutine

! This tests an error in the parser.
! procedure foo()
