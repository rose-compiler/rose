 ! This test code demonstrates that the scope of the implied do loop variable is 
 ! not local to the implied do loop, but instead is the scope of the statement
 ! in which the implied do loop is present.

 ! If "implicit none" is used, then the declaration of "j" is required.
   implicit none

 ! This is only required because of a bug in gFortran (should not be required).
   integer j

 ! Implied do loop for the direct data initialization in a type statement
 ! real, dimension (3) :: a = (/(j,j=1,3)/)
   real, dimension (3) :: a = (/(j**2+j,j=1,3)/)

 ! This next implied do loop demonstrates that we need to have OFP communicate 
 ! the implied do loop variable (not fixed in OFP).
   integer, parameter :: i = 1
   real, dimension (3) :: b = (/(j**2+i,j=1,3)/)

end

