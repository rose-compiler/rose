   ! I think this is the same problem as test2007_107.f90 
     DOUBLE PRECISION FIELD(5,5) 

   ! This appears to be an arror (as reported by gfortran)
     DATA ((FIELD(I,J),I=1,J), J=1,5) / 15 * -1.0D0 /
END
