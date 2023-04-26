PROCEDURE generic_namequal IS
     GENERIC
          TYPE T IS RANGE <>;
     PACKAGE GPACK IS
          procedure zero(v: out t);
       
          PACKAGE PACKA RENAMES GPACK;
     END GPACK;

     package body GPACK is
       PACKAGE PACKB RENAMES GPACK;
       
       procedure zero(v: out t) is null;
     end;
BEGIN
     null;
END;
