subroutine example
   ! implied do loop to initialize a to values: 1,2,3,4,5,6,7,8,9,10
   ! integer :: a(10)= 42
   ! integer :: a = 42
   ! integer :: b(10)=(/(j,j=1,10)/)

   ! These two forms of syntax are equivalent... but they are not unparsed the same!
   ! integer :: b(10)=(/(j,j=1,9,2),(j,j=2,11,2)/)
   ! integer, dimension(10) :: a = 0,b=(/(j,j=1,9,2),(j,j=2,11,2)/)

   ! integer :: a = 0,b(10)=(/(j,j=1,9,2),(j,j=2,11,2)/)
     integer :: a = 0,b(10)=(/(j,j=1,9,2),(j,j=2,11,2)/)

   ! write (1) b (i,i=1,10)

end subroutine example
