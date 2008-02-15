c 12/11/2007
c simplest fortran 77 program
      PROGRAM HELLO
      DO 15 I=1,5
      WRITE(UNIT=*, FMT=*) 'Hello, world'
15    CONTINUE
      END
