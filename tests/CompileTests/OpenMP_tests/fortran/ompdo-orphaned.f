! Orphaned omp do
! Liao 1/12/2011
      program  main
!      implicit none
!      external foo
!$omp parallel
      call foo()
!$omp end parallel      
      stop
      end

      subroutine foo()
      integer i, j;
      include 'omp_lib.h'
!$omp do private(j) schedule(static,2)
      do i = 1, 11
        j = omp_get_thread_num();
        print *, "Iteration ", i, " by thread:", j
      enddo

      end
