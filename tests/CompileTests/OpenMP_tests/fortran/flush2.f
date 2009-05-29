      subroutine f1(q)
      common /data/ x, p
      integer, target :: x
      integer, pointer :: p
      integer q
      q = 1
!$OMP FLUSH (X,P,Q)  ! bug, cannot find symbol for x since it is attached to the
      !subroutine, not the inner scope
      end subroutine f1

      subroutine f2(q)
      common /data/ x, p
      integer, target :: x
      integer, pointer :: p
      integer q
!$OMP BARRIER
      q = 2
!$OMP BARRIER
      ! a barrier implies a flush
      ! X, P and Q are flushed
      ! because they are shared and accessible
      end subroutine f2

      integer function g(n)
      common /data/ x, p
      integer, target :: x
      integer, pointer :: p
      integer n
      integer i, j, sum
      i = 1
      sum = 0
      p = 1
!$OMP PARALLEL REDUCTION(+: SUM) NUM_THREADS(10)
      call f1(j)
      ! I, N and SUM were not flushed
      ! because they were not accessible in F1
      ! J was flushed because it was accessible
      sum = sum + j
      call f2(j)
      ! I, N, and SUM were not flushed
      ! because they were not accessible in f2
      ! J was flushed because it was accessible
      sum = sum + i + j + p + n
!$OMP END PARALLEL
      g = sum
      end function g

      program a22
      common /data/ x, p
      integer, target :: x
      integer, pointer :: p
      integer result, g
      p => x
      result = g(7)
      print *, result
      end program a22
