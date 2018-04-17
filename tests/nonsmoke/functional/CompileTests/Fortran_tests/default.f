      SUBROUTINE A28(a)
      INTEGER a
      INTEGER y, z(1000)
      INTEGER i, j
      i = 1
!$OMP PARALLEL DEFAULT(NONE) PRIVATE(A) SHARED(Z) PRIVATE(J)
      j = 1
      a = z(j) ! O.K. - A is listed in PRIVATE clause
!$OMP END PARALLEL
      END SUBROUTINE A28
