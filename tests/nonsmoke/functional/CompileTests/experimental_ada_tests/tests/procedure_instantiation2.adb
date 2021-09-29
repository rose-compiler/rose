PROCEDURE Procedure_Instantiation2 IS

     PACKAGE PKG IS
          GENERIC
          PROCEDURE P20 ;        -- OK.
     END PKG;

     PACKAGE BODY PKG IS
          PROCEDURE P20 IS
          BEGIN
            null;
          END P20;
     END PKG;

     PACKAGE INSTANCES IS
          PROCEDURE NP20 IS NEW PKG.P20;
     END INSTANCES;

BEGIN
  null;
END Procedure_Instantiation2;
