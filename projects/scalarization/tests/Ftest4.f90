implicit none
integer,parameter :: n=10
integer :: i
!$rose scalarization
integer,dimension(n) :: a1,a2,a3
 
integer,dimension(n) :: b1,b2
integer,dimension(n) :: c
 
do i=1,n
  b1(i)=i+3
  b2(i)=i-2
enddo
 
!$acc parallel loop gang vector
do i=1,10
  a1(i)=b1(i)
  a2(i)=b1(i)+b2(i)
  a3(i)=b1(i)-2*b2(i)
  c(i)=2*a1(i)+3*a2(i)+a3(i)+b1(i)
enddo
 
end program
