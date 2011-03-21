! Rice test
! extraneous parentheses are generated in a keyword argument if the argument is a 
! component of a derived type.  The compiler will flag this as an error if the
! corresponding formal parameter is INTENT = OUT
program keyword_arg_extra_paren

 type t
   integer n
 end type
 
 type(t) y
 
 call g(k=y%n)  ! translated as CALL g(k = ( y%n ))
  
 contains
 
   subroutine g(k)
     integer , INTENT(OUT):: k !formal parameter must be INTENT = IN/OUT
   end subroutine
 
end program
