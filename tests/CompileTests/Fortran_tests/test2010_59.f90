 ! Implied do loop for the direct data initialization in a type statement
 ! real, dimension (3) :: a = (/(j,j=1,3)/)
   real, dimension (3) :: a = (/(j**2+j,j=1,3)/)

   integer, parameter :: i = 1
   real, dimension (3) :: b = (/(j**2+i,j=1,3)/)

end

