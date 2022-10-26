PROCEDURE Mangle_DiscTypes IS

     PACKAGE PKG IS
          TYPE PRIVA (D : INTEGER := 0) IS PRIVATE;
          PRA1 : CONSTANT PRIVA (1);

     PRIVATE
          TYPE PRIVA (D : INTEGER := 0) IS null record;
          PRA1  : CONSTANT PRIVA (1) := (D => (1));
     END PKG;

     TYPE RECA (D : INTEGER := 0) IS null record;

     RA1 : CONSTANT RECA (1) := (D => (1));
BEGIN
  null;
END Mangle_DiscTypes;
