SUBROUTINE DGEMM ( TRANSA, TRANSB, M, N, K, ALPHA, A, LDA, B, LDB, BETA, C, LDC )
!     .. Scalar Arguments ..
     CHARACTER*1        TRANSA, TRANSB
     INTEGER            M, N, K, LDA, LDB, LDC
     DOUBLE PRECISION   ALPHA, BETA
!     .. Array Arguments ..
     DOUBLE PRECISION   A( LDA, * ), B( LDB, * ), C( LDC, * )
     RETURN
END
