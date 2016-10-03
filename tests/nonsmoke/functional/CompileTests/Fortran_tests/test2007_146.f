C Example of Fortran II program:
C This program has one data card input, with simple zero-data edit check. 
C If one of the input values is zero, then the program will end with an 
C error code of "STOP 1" in the job control card listing following the 
C execution of the program. Normal output will be one line printed with 
C A, B, C, and AREA on the "report" page following the compilation with 
C no specific units are stated; and with a normal "STOP" in the job card 
C listing.

C AREA OF A TRIANGLE WITH A STANDARD SQUARE ROOT FUNCTION 
C INPUT - CARD READER UNIT 5, INTEGER INPUT
C OUTPUT - LINE PRINTER UNIT 6, REAL OUTPUT
C INPUT ERROR DISPAY ERROR OUTPUT CODE 1 IN JOB CONTROL LISTING
      INTEGER A,B,C
      READ(5,501) A,B,C
  501 FORMAT(3I5)
      IF(A.EQ.0 .OR. B.EQ.0 .OR. C.EQ.0) STOP 1
      S = (A + B + C) / 2.0
      AREA = SQRT( S * (S - A) * (S - B) * (S - C) / 2.0)
      WRITE(6,601) A,B,C,AREA
  601 FORMAT(4H A=,I5,5H B=,I5,5H C= ,I5,8H AREA=,F10.2,12HSQUARE)
      STOP
      END
