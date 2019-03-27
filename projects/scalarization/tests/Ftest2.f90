      subroutine test
      integer,parameter :: vecsize = 10
      real(8),dimension(vecsize):: array1
!$rose scalarization
      real(8),dimension(vecsize):: array2
      do i=1,vecsize
        array2(i) = array1(i)
      enddo
      end subroutine test
