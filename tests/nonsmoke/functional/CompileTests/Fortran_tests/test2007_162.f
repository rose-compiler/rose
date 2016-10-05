! An example program from Liao
      PROGRAM TRIANG
      WRITE(*,*) 'AREA is ', AREA3(10.0,8.2,9.9)
! implicit data type by variable name, I, N integer, all other are real
! warning of mismatched types for parameter by g77
!      WRITE(*,*) 'AREA is ', AREA3(10,8,9)
      END

      FUNCTION AREA3(A,B,C)
      S = (A+B+C)/2.0
      AREA3 = SQRT (S*(S-A)*(S-B)*(S-C))
      END
