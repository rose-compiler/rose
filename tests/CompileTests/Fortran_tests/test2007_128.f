      SUBROUTINE DGEMM ( M, N, K, A, LDA, B, LDB, 
     & C, LDC )
!     .. Scalar Arguments ..
      INTEGER            M, N, K, LDA, LDB, LDC
      DOUBLE PRECISION   ALPHA, BETA
!     .. Array Arguments ..
      DOUBLE PRECISION   A( LDA, * ), B( LDB, * ), C( LDC, * )
      RETURN
      END
