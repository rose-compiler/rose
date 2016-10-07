! P01-forward-ref-derived-type.f90
! If a pointer component of a derived type makes a forward reference
! to another derived type, it is unparsed with type 'integer'. 

program p

   type :: t1
      type(t2), pointer :: c  ! type of 'c' becomes 'integer'

    ! Note that this (below) is not legal code since t2 has not ben seen yet.
    ! So we only have to deal with the indirect ways in which previously 
    ! unseen types can be referenced.
    ! type(t2) :: d  ! type of 'c' becomes 'integer'
   end type

   type :: t2
      real ::  x
   end type

 ! Added this code to test the use of the correct type.
   type(t1) a
   real :: b = 5.0
   a%c%x = b

end program
