***************************************************      
      program main 
      implicit none 

      double precision u(500,500),f(500,500)

      call initialize (500,500,u,f)
      stop
      end 

***************************************************      
      subroutine initialize (n,m,u,f) 
      implicit none 
     
      integer n,m
      double precision u(n,m),f(n,m)
      
      integer i,j

!$omp parallel do
      do j = 1,m
         do i = 1,n
            u(i,j) = 0.0 
            f(i,j) = 1.2
         enddo
      enddo
!$omp end parallel do

      return 
      end 
