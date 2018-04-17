      program  main
      implicit none
      include 'omp_lib.h'
      integer lower, upper, stride
      integer i, j

      lower = 9
      upper = 0
      stride = -3
!$omp parallel do private (j)
      do i = lower, upper, stride
        j = omp_get_thread_num();
        print *, "Iteration ", i, " by thread:", j
      enddo  
      end
