! label is dropped
      integer I,Y(10)
      DO 1 I = 1,10
    1 Y(I) = 0.0D0
      WRITE (UNIT=LOUT, FMT=20) I
   30 FORMAT ('hello')
   20 format (' data:  I=',f9.2 )
      Y(I) = 0.0D0
      END
