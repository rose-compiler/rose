      SUBROUTINE DTRTRI( UPLO, DIAG, N )
*     .. Scalar Arguments ..
      CHARACTER          DIAG, UPLO
      INTEGER            N
*     ..
*     .. Array Arguments ..
      INTEGER            NB
*     ..
*     .. External Functions ..
      INTEGER            ILAENV
      EXTERNAL           LSAME, ILAENV
*     ..
*
*     Determine the block size for this environment.
*
      NB = ILAENV( 1, 'DTRTRI', UPLO // DIAG, N, -1, -1, -1 )

      RETURN
*
*     End of DTRTRI
*
      END
