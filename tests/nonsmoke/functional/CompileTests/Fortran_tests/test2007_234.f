! A related problem is that the current handling for loop terminal
! statement is hard to justify: An END DO is inserted and the label is
! moved from CONTINUE to the END DO.

      PROGRAM HELLO
      DO 15 I=1,5
      WRITE(UNIT=*, FMT=*) 'Hello, world'
15    CONTINUE
      END
