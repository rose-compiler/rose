      program  main
      implicit none
      include 'omp_lib.h'
      call foo (9,0, -3)
      end

      subroutine foo (lower, upper, stride)
      integer lower, upper, stride
      integer i
!$omp parallel do
      do i = lower, upper, stride
        print *, "Iteration ", i
      enddo  
      end
