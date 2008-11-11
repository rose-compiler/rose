type real_num
   real :: x
end type

contains

function foo0()
  ! A redundant specification is allowed for primative type, but not for derived types
  integer :: foo0
end function

integer function foo1()
  ! A redundant specification is allowed for primative type, but not for derived types
  ! integer :: foo1
end function

! Return type specification before the function works fine
type(real_num) function foo2()
  ! A redundant specification is not allowed for derived types
  ! type(real_num) :: foo2
end function

end

