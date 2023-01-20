PROCEDURE resolve_more_overloads IS

     PACKAGE P IS
          TYPE PARENT IS NEW INTEGER;

          FUNCTION CREATE (X : INTEGER) RETURN PARENT;
     END P;

     TYPE T IS NEW P.PARENT;

     PACKAGE BODY P IS
          FUNCTION CREATE (X : INTEGER) RETURN PARENT IS
          BEGIN
               RETURN PARENT (X);
          END CREATE;
     END P;

BEGIN
  if CREATE(30) = CREATE(29) then
    null;
  end if;
END;
