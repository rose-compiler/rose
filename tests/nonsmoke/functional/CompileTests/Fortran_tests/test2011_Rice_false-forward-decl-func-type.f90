! Rice test
!
program false_fwd_decl_func_type

  type t
    integer :: m
  end type t
  
contains

  type(t) function a()
  
    type t2
      integer :: j
    end type t2
    
    type t
      logical :: b
    end type t

    a%b = .true.
    
  end function a

end program
