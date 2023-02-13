
PROCEDURE ScopeQual2 IS

     PACKAGE PKG IS
          TYPE PARENT IS ARRAY (INTEGER RANGE <>) OF FLOAT;

          SUBTYPE PARTPARENT is PARENT(0..31);
          PROCEDURE TEST (DUMMY : PARTPARENT);
     END PKG;

     TYPE T IS NEW PKG.PARENT (5 ..7);

     SUBTYPE SUBPARENT IS PKG.PARENT (1 .. 32);

     TYPE S IS NEW SUBPARENT;

     X  : T := (OTHERS => 2.0);
     Y  : S := (OTHERS => 2.0);
     Z  : SUBPARENT := (OTHERS => 2.0);

     PACKAGE BODY PKG IS
          PROCEDURE TEST (DUMMY : PARTPARENT) IS
          BEGIN
               null;
          END TEST;
     END PKG;

BEGIN
  TEST(X);
  TEST(Y);
  PKG.TEST(Z);
END ScopeQual2;
