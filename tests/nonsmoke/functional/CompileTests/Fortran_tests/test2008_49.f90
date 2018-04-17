type real_num
   real :: x
end type

contains

! This is an error since the function is assigned an implicit type
! and the declaration of the return type using the older syntax does
! not reset the type of the function.
function foo1()
    type(real_num) :: foo1
end function

! Return type specification before the function works fine
type(real_num) function foo2()
  ! A redundant specification is not allowed
  ! type(real_num) :: foo2
end function

end

