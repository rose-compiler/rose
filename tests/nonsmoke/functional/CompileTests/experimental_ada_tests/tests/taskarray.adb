procedure TaskArray is
          G : Integer := 0;

          TASK T IS
               ENTRY E;
          END T;

          TASK BODY T IS
               TYPE RT IS
                    RECORD
                         M : BOOLEAN := BOOLEAN'VAL(G);
                         N : CHARACTER := CHARACTER'VAL(G);
                    END RECORD;

               TYPE ART IS ARRAY (1 .. 1) OF RT;
          begin
            null;
          END T;

begin
  null;
end TaskArray;
