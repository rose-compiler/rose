! Rice test
! Defining a component with the same name as that of an existing function,
! causes the function to be unparsed without its return type.

module lost_func_return_type

  type t
    integer :: k
  end type t
  
contains

type(t) function b()  ! =>  FUNCTION B()
 b%k = 5
end function b

integer function c()
  type t2
    logical :: b  ! this component has the same name as the function b previously defined
  end type t2
end function 

end module
