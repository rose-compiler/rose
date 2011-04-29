      subroutine initialize (n,m,u)
      implicit none

      integer n,m
      double precision u(n,m)
      integer i,j

!$omp parallel do 
      do j = 1,m
         do i = 1,n
            u(i,j) = 0.0
         enddo
      enddo
!$omp end parallel do

      return
      end

