! This fails in ROSE because "I" has not been previously declared and we get confused.
      INTEGER ODD(10)
      DATA (ODD(I),I=2,10,2) /5*0/
      END
