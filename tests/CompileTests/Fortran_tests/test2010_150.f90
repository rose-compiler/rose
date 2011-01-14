! P01-forward-ref-derived-type.f90
! If a pointer component of a derived type makes a forward reference
! to another derived type, it is unparsed with type 'integer'. 

program p

   type :: t1
      type(t2), pointer :: c  ! type of 'c' becomes 'integer'
   end type

   type :: t2
      real ::  x
   end type

end program
