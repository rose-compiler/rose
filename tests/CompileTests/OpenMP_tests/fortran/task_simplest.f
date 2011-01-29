******************************************      
* Simplest OMP task example, no variables 
* Liao 1/25/2010
******************************************
      program main
      include 'omp_lib.h'
      integer i

!$omp parallel
!$omp single
      print *, 'using ', omp_get_num_threads(), ' threads' 
      do i = 1, 10
!$omp task 
      print *, 'Executed by thread ', omp_get_thread_num()
!$omp end task
      continue ! use a no-op stmt to get comment position right, a workaround
      enddo  
!$omp end single
!$omp end parallel
      end 
