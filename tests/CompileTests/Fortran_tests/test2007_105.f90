   ! test2007_143.f90 presents a simple example of this bug.
     PARAMETER (NX = 800, NY = 360, NTOTAL = NX * NY) 
     DOUBLE PRECISION SCREEN(NX,NY), ZERO 
     PARAMETER (ZERO = 0.0D0) 
     DATA SCREEN / NTOTAL * ZERO /
END
