! Rice test

program forward_ref_with_sub_2

contains
   subroutine g(a)
!   	  type(t2) :: a ! Error: Derived type 't2' is being used before it is defined
   	     
   	  type :: t1
        type(t2), pointer :: c  ! type of 'c' becomes 'integer'
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
