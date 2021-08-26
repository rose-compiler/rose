procedure Discriminated_Record is
  TYPE REC2 (I : INTEGER) IS
       RECORD
            CASE I IS
                 WHEN 1 =>
                      null;
                 WHEN OTHERS =>
                      A2 : integer;
                      A3 : long_integer;
            END CASE;
       END RECORD;
  R2 : REC2(2);
begin 
  null;
end Discriminated_Record;
