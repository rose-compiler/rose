c 12/11/2007
c by Liao
c example for function definition and calling
c note:
c   add return type, argument type specification is
c   no forward declaration is needed by default
c------------------------------------------------
c definition of function
      REAL FUNCTION AREA3(A,B,C)
      REAL A,B,C
      S = (A+B+C)/2.0
c return value is assigned to a variable named as the function name   
      AREA3 = SQRT (S*(S-A)*(S-B)*(S-C))
      END

      PROGRAM TRIANG
      WRITE(*,*) 'AREA is ', AREA3(10.0,8.2,9.9)
c implicit data type by variable name, I, N integer, all other are real
c warning of mismatched types for parameter by g77
c      WRITE(*,*) 'AREA is ', AREA3(10,8,9)
      END

