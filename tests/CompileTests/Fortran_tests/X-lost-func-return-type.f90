! defining a component with the same name as that of an existing function,
! removes the return type of this function.
module m_lost_return_type

  type t
    integer :: k
  end type t
  
contains

type(t) function b()  ! the type info will be lost
end function b

integer function c()
  type t2
    logical :: b  ! this component has the same name as the function b previously defined
  end type t2
end function 

end module
