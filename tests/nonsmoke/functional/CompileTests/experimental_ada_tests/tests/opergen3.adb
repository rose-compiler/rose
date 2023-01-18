
PROCEDURE opergen3 IS

     PACKAGE P IS
          TYPE T1 IS DELTA 2.0**(-4) RANGE -100.0 .. 100.0;
     END P;

     X1 : P.T1 := 6.0;
     X2 : P.T1 := 2.0;

BEGIN
     X1 := P.T1 (X1 * X2);
     X2 := P.T1 (X1 / X2);

     X1 := P.T1 (STANDARD."*" (X1,X2));
     X2 := P.T1 (STANDARD."/" (X1,X2));

     X1 := P.T1 ("*" (X1,X2));
     X2 := P.T1 ("/" (X1,X2));
END opergen3;
