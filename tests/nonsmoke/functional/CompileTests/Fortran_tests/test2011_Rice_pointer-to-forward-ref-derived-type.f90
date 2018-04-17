! Rice test
! When a derived type is correctly used before it is defined in a pointer-to-T,
! it is unparsed as 'integer'.

program pointer_to_forward_ref_derived_type
contains

  ! probably not important that the forward ref is in an internal function
  
   subroutine g(a)
   	     
   	  type :: t1
        type(t2), pointer :: c  ! type of 'c' unparses as 'integer'
      end type
      
      type :: t2
        character :: z
      end type
      
      type(t2) :: b
      type(t1) :: y
	  type(t2) :: a
	  
      allocate(y%c)
	  b%z = 'w'
      y%c%z = 'v' 
      a%z = '0'
            
   end subroutine
   	  
end program
