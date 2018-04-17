! Rice test
! extraneous parentheses are generated in a keyword argument if the argument is a 
! an array.  The compiler will flag this as an error if the
! corresponding formal parameter is INTENT = OUT
program keyword_arg_extra_paren_2

REAL, DIMENSION(2,3) :: S
 
 call g(R = S(:,1))  ! translated as CALL g(R = ( S(:,1) ) )
  
 contains
  
 SUBROUTINE g(R)
   REAL, DIMENSION(2), INTENT(OUT) :: R
 END SUBROUTINE
 
 
end program
