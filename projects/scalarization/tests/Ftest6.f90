      subroutine test
      integer,parameter :: vecsize = 10
!$rose scalarization(2)
      real(8),dimension(5,vecsize,10):: array1
      do k=1,10
      do i=1,vecsize
      do j=1,5
          array1(i,j,k) = 1.
      enddo
      enddo
      enddo
      end subroutine test
