! Rice test
! function return type is incorrectly generated when the function is called before its definition
program wrong_func_type

    type t  
      logical :: b
    end type t
    
contains

  function func()     ! return type is incorrectly generated as REAL.
    type(t) :: func    
    func%b = .true.
  end function
  
end program 