! Array type
      SUBROUTINE foo(N)
      integer N
      double precision array(N)
!$rose_outline      
c$rose_outline
*$rose_outline
      DO 15 I=1,5
       array(I) = 0.0
15    CONTINUE
      A=1
      END
