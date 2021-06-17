

PROCEDURE Inherited_Primitive_Operation IS
     PACKAGE Inner IS
          TYPE PARENT IS ARRAY (INTEGER RANGE <>) OF float;

          FUNCTION CREATE ( F, L  : INTEGER;
                            TYPETAG : PARENT   -- TO RESOLVE OVERLOADING.
                          ) RETURN PARENT;

     END Inner;

     TYPE T IS NEW Inner.PARENT (5 .. 7);

     X : T               := (OTHERS => 2.0);

     PACKAGE BODY Inner IS

          FUNCTION CREATE
             ( F, L : INTEGER;
               TYPETAG : PARENT
             ) RETURN PARENT
          IS
               A : PARENT (F .. L) := (others => 0.0);
          BEGIN
               RETURN A;
          END CREATE;

     END Inner;

BEGIN
  -- CREATE MUST NOT HAVE A PREFIX
  X := CREATE(2, 4, X);
END Inherited_Primitive_Operation;
