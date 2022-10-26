PROCEDURE GeneX IS

     GENERIC
          PACKAGE PAC IS
               EXC : EXCEPTION;
          END PAC;

BEGIN
     -------------------------------------------------------------------
     DECLARE
          PACKAGE PAC1 IS NEW PAC;
     BEGIN
        RAISE PAC1.EXC;

        EXCEPTION
           WHEN OTHERS => RAISE PAC1.EXC;
     END;

END GeneX;
