! This appears to be a F2003 specific construct, so gfortran will 
! not handle it properly UNLESS using F2003 mode (-std=f2003).
! and given the correct .f03 filename suffix.
PROGRAM ASSOCIATE_EXAMPLE
     REAL :: X,A
     A = 0.4

   ! The associate construct is F2003 specific
     ASSOCIATE ( Z => A )
          X = Z
     END ASSOCIATE

END PROGRAM ASSOCIATE_EXAMPLE
