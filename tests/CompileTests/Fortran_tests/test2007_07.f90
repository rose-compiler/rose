! This shows how only the c_actions that are specific to subroutines are not called.
program main
end program main

! This function is implicitly typed
function foo(i)
   foo = 0
end function
