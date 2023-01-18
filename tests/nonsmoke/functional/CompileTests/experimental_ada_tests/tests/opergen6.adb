PROCEDURE opergen6 IS

     PACKAGE PACK1 IS
          TYPE PRIVY IS RANGE 0..127;
          ZERO : CONSTANT PRIVY := 0;
     END PACK1;

     TYPE ARR_PVT IS ARRAY(POSITIVE RANGE <>) OF PACK1.PRIVY;

     AV1  : ARR_PVT(1..8) := (OTHERS => PACK1.ZERO);
     XAV1 : ARR_PVT RENAMES AV1(5..7);

BEGIN
          IF PACK1."/=" (XAV1(2), PACK1.ZERO) THEN
               null;
          END IF;
END opergen6;
