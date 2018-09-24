      subroutine test
!pragma array1
      integer,parameter :: vecsize = 10
      real(8),dimension(vecsize):: array1,array2
      do i=1,vecsize
        array2(i) = array1(i)
      enddo
      end subroutine test
