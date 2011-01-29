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
        item(i) = 11-i
      enddo  

!$omp parallel
!$omp single
      print *, 'using ', omp_get_num_threads(), ' threads' 
      do i = 1, 10
!$omp task 
      call process(i,item)
!$omp end task
      continue ! use a no-op stmt to get comment position right, a workaround
      enddo  
!$omp end single
!$omp end parallel
      end 

******************************************
      subroutine process(input,item)
      include 'omp_lib.h'
      integer input
      integer item(10)
      print *, 'idx ', input,'-> ', item(input)
      print *, 'by thread ', omp_get_thread_num()
      end

