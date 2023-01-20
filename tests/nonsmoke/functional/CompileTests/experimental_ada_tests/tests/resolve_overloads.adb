PROCEDURE resolve_overloads IS

     PACKAGE P IS
          TYPE NUM IS NEW INTEGER;

          FUNCTION CREATE (X : INTEGER) RETURN NUM;
     END P;

     TYPE T IS NEW P.NUM;

     X : T;

     PACKAGE BODY P IS
          FUNCTION CREATE (X : INTEGER) RETURN NUM IS
          BEGIN
               RETURN NUM (X);
          END CREATE;
     END P;

BEGIN
  X := CREATE(30);
END;
