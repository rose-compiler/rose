
PROCEDURE scopequal5 IS

     PACKAGE P IS
          TYPE BASE IS ARRAY (INTEGER RANGE <>) OF FLOAT;

          PROCEDURE CREATE (el : out BASE);
     END P;

     TYPE DERIVED IS NEW P.BASE (5 .. 7);

     X : DERIVED := (OTHERS => 2.0);

     PACKAGE BODY P IS
          PROCEDURE CREATE(el : out BASE) IS
          BEGIN
            null;
          END CREATE;
     END P;


BEGIN
  CREATE(X);
END scopequal5;
