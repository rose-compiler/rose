******************************************      
* Simplest OMP task example
* Liao 1/21/2010
******************************************
      program main
      include 'omp_lib.h'
      integer i
      integer item(10)
      external process
      do i = 1, 10
        item(i) = i
      enddo  

!$omp parallel
!$omp single
      print *, 'using ', omp_get_num_threads(), ' threads' 
      do i = 1, 10
!$omp task 
      call process(item(i))
!$omp end task
      enddo  
!$omp end single
!$omp end parallel
      end 

******************************************
      subroutine process(input)
      include 'omp_lib.h'
      integer input
      print *, 'Item ', input, ' by thread ', omp_get_thread_num()
      end

