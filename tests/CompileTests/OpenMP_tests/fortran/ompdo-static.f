      program  main
      implicit none

      include 'omp_lib.h'

      integer i, j;
!$omp parallel do private(j) schedule(static,2)
      do i = 1, 10
        j = omp_get_thread_num();
        print *, "Iteration ", i, " by thread:", j
      enddo  

      end
