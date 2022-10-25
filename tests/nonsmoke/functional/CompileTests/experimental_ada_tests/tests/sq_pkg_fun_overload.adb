
-- tests overloading of package P1 with procedure P1
-- and the refering back to the package using scope-qualification
-- DD.P1.
PROCEDURE SQ_PKG_FUN_OVERLOAD IS
BEGIN
     DD:
     DECLARE

          PACKAGE P1 IS
               PROCEDURE P1 ;
               PACKAGE PQ1 RENAMES DD.P1;
               PROCEDURE P98 RENAMES P1;
          END P1;

          PACKAGE BODY P1 IS
               PROCEDURE P1 IS BEGIN NULL; END P1;
          END P1;

     BEGIN
          NULL;
     END DD;
END SQ_PKG_FUN_OVERLOAD;
