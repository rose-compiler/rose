PROCEDURE BlockReference IS

BEGIN
     Blk: DECLARE
          I : INTEGER := 2;

          TYPE REC (X : INTEGER := Blk.I) IS RECORD
               V : INTEGER := I;
          END RECORD;

     BEGIN
       null;
     END Blk;

     null;
END BlockReference;
