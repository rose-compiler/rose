      subroutine test
      integer,parameter :: vecsize = 10
!$rose scalarization
      real(8),dimension(vecsize):: array1
      do i=1,vecsize
        array1(i) = 1.
      enddo
      end subroutine test
