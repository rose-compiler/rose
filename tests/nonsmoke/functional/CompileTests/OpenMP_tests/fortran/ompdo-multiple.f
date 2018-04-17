! multiple omp do loops      
      program  main
      implicit none

      include 'omp_lib.h'

      integer i, j;
!$omp parallel


!$omp do private(j) schedule(static,2)
      do i = 1, 10
        j = omp_get_thread_num();
        print *, "Iteration ", i, " by thread:", j
      enddo  

!$omp do private(j) schedule(dynamic,3)
      do i = 1, 10
        j = omp_get_thread_num();
        print *, "Iteration ", i, " by thread:", j
      enddo  

!$omp end parallel 

      end
