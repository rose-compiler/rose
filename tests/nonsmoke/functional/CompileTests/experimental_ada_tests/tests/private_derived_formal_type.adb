
PROCEDURE Private_Derived_formal_Type IS

     C3 : CONSTANT INTEGER := 3;

     TYPE REC (DISC : INTEGER) IS
          RECORD
               NULL;
          END RECORD;

     GENERIC
          TYPE ACC_REC IS ACCESS REC;
     PACKAGE P IS
          TYPE DER1 IS PRIVATE;
     PRIVATE
          TYPE DER1 IS NEW ACC_REC(C3);
     END P;

BEGIN
  null;
END;
