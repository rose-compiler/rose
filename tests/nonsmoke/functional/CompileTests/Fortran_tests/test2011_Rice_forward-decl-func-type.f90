! Rice test
! The scope of a derived type extends to the type-spec of the enclosing function-stmt.

type(t) function a()

  type t
    integer :: c
  end type t
  
  a%c = 0
  
end function 
