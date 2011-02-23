! One dimensional array as shared variable       
      subroutine initialize (n,u)
      implicit none

      integer n
      double precision u(n)
      integer i

!$omp parallel do 
      do i = 1,n
         u(i) = 0.0
      enddo
!$omp end parallel do

      return
      end

