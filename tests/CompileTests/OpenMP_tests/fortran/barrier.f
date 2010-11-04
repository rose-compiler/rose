      subroutine f2(q)
      integer q
!$OMP BARRIER
      q = 2
!$OMP BARRIER
      end subroutine f2

      program a22
      end program a22
