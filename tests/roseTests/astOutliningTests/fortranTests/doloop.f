c 12/11/2007
c simplest fortran 77 program
      PROGRAM HELLO
!$rose_outline      
c$rose_outline
*$rose_outline
      DO 15 I=1,5
      WRITE(UNIT=*, FMT=*) 'Hello, world'
15    CONTINUE
      A=1
      END
