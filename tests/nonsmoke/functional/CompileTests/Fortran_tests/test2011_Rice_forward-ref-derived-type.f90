! Rice test
! If a pointer component of a derived type makes a forward reference
! to another derived type, it is unparsed with type 'integer'. 

program forward_ref_derived_type

   type :: t1
      type(t2), pointer :: c  ! type of 'c' becomes 'integer'
      type(t2), pointer :: d  !!!!
      type(t99), pointer :: x99
      type(t1), pointer :: y77
   end type

   type :: t2
      real ::  x
      type(t99), pointer :: z99
      type(t1) :: wxyz
   end type
   
   type :: t99
   	  type(t2) :: a
   	  type(t1) :: b
   	  type(t99), pointer :: p99
   end type
   
   type(t1) a
   a%c%x = 1.0

end program
