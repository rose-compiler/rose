C     Test code extracted from test2007_227.f which was failing after fixing strings and a lot of other details...
      SUBROUTINE X_MCFIT ()

      COMMON /MCVERS/ PRVERS, PREC, FILVER
      CHARACTER*16 FILVER, PRVERS, PREC

      INTEGER CKLSCH
      EXTERNAL CKLSCH

C     PRVERS(1:CKLSCH(PRVERS)+1) = 'xxx'
      PRVERS(1:3) = 'xxx'

      RETURN
      END

      SUBROUTINE MCFIT ()

      COMMON /MCVERS/ PRVERS, PREC, FILVER

      COMMON /MCCON/ PI, RU, PATM, BOLTZ, EPSIL, TLOW, DT, P, EMAXL,
     1                EMAXE, EMAXD, EMAXTD, FCON
C
      CHARACTER*80 LINE

      CHARACTER*16 FILVER, PRVERS, PREC

      RETURN
      END
