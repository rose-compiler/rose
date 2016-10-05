! Rice test
! function return type is incorrectly generated when the function is called before its definition

program Rice_wrong_func_type

  type t  
    integer :: c
  end type t
    
contains

  function func()     ! return type unparsed as 'real'
    type(t) :: func    
    func%c = 0
  end function
  
end program
