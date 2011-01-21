      program  main
      implicit none
!     include 'omp_lib.h'

      integer i, sum1;
      sum1 = 0
!$omp parallel do reduction(+:sum1)
      do i = 1, 100
        sum1 = sum1 + i
      enddo  
      print *, "sum of 1 to 100 is:", sum1

      end
