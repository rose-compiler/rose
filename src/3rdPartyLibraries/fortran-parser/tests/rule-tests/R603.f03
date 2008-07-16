! Test designator, which is:
!      designator  is  object-name
!                  or  array-element
!                  or  array-section
!                  or  structure-component
!                  or  substring
! 
! Full tests for each of the alternatives is done separately.  
!
! designator tested as part of a variable (R601), which is used as part of 
! an assignment-stmt (R734).
a = 1
a(1) = 1
a(1:2) = 1
a%b = 1

end

