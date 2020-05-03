      subroutine test
      integer,parameter :: vecsize = 10
!$rose scalarization
      real(8),dimension(vecsize,5):: array1
      do j=1,5
        do i=1,vecsize
          array1(i,j) = 1.
        enddo
      enddo
      end subroutine test
