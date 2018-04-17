      SUBROUTINE A28(A)
      INCLUDE "omp_lib.h" ! or USE OMP_LIB
      INTEGER A
      INTEGER X, Y, Z(1000)
      COMMON/BLOCKX/X
      COMMON/BLOCKY/Y
      COMMON/BLOCKZ/Z
!$OMP THREADPRIVATE(/BLOCKX/)
      INTEGER I, J
      i = 1
!$OMP PARALLEL DEFAULT(NONE) PRIVATE(A) SHARED(Z) PRIVATE(J)
      J = OMP_GET_NUM_THREADS();
      ! O.K. - J is listed in PRIVATE clause
      A = Z(J) ! O.K. - A is listed in PRIVATE clause
      ! - Z is listed in SHARED clause
      X = 1 ! O.K. - X is THREADPRIVATE
      !Z(I) = Y ! Error - cannot reference I or Y here
!$OMP DO firstprivate(y)
      DO I = 1,10
      Z(I) = Y ! O.K. - I is the loop iteration variable
      ! Y is listed in FIRSTPRIVATE clause
      END DO
      !Z(I) = Y ! Error - cannot reference I or Y here
!$OMP END PARALLEL
      END SUBROUTINE A28
