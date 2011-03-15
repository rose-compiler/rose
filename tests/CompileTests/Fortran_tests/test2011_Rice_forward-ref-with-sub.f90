! THIS PROGRAM IS CORRECT BUT FAILS ON GFORTRANS OLDER THAN 4.4.1
! Rice test

program forward_ref_with_sub

   type :: t2
      real ::  x
   end type

contains
   subroutine g(a)
   	  type(t2) :: a
   	     
   	  type :: t1
        type(t2), pointer :: c  
      end type
      
      type :: t2
        character :: z
      end type
      
      type(t2) :: b
      type(t1) :: y
      
     allocate(y%c)
     a%x = 0
	 b%z = 'w'
!      y%c%z = 'v' ! Error: 'z' is not a member of the 't2' structure
            
   end subroutine

   	  
end program
