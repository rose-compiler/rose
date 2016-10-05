C This program dates to the original Fortran manual (1956)
C [ Note that I commented out the FREQUENCY statement and added an END statement ]
C     PROGRAM FOR FINDING THE LARGEST VALUE
C    X     ATTAINED BY A SET OF NUMBERS
      DIMENSION A(999)
C     FREQUENCY 30(2,1,10), 5(100)
      READ 1, N, (A(I), I = 1,N)
    1 FORMAT (I3/(12F6.2))
      BIGA = A(1)
    5 DO 20 I = 2,N
   30 IF (BIGA-A(I)) 10,20,20
   10 BIGA = A(I)
   20 CONTINUE
      PRINT 2, N, BIGA
    2 FORMAT (22H1THE LARGEST OF THESE I3, 12H NUMBERS IS F7.2)
      STOP 77777
      END
