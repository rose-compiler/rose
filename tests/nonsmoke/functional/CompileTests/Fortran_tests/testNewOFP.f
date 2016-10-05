*      subroutine initialize (n,m,alpha,dx,dy,u,f) 
      subroutine initialize (n,m) 
******************************************************
* Initializes data 
* Assumes exact solution is u(x,y) = (1-x^2)*(1-y^2)
*
******************************************************
      implicit none 
     
      integer n,m

      integer i,j,xx

!$omp parallel do private(xx)
      do j = 1,m
         do i = 1,n
         enddo
      enddo
!$omp end parallel do

      return 
      end 
