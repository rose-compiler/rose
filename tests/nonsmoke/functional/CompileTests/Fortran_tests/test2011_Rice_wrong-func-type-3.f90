! Rice test
! function return type is incorrectly generated when the function is called before its definition
program wrong_func_type_3
    logical :: x
    x = func()
    
contains

  function func()     ! return type is incorrectly generated as REAL.
   logical :: func    
   func = .true.
  end function
  
end program
