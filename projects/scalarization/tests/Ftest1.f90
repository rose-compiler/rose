      subroutine test
      integer,parameter :: vecsize = 10
!pragma privatization
      real(8),dimension(vecsize):: array1
      do i=1,vecsize
        array1(i) = 1.
      enddo
      end subroutine test
