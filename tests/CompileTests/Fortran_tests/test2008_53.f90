! This is an error since the function is assigned an implicit type
! and the declaration of the return type using the older syntax does
! not reset the type of the function.
function foo2()
    integer foo2
    foo2 = 1
end function
