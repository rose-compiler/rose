separate (Rep_E_Queue)
task body Queue1 is
   My_Bool : boolean := false;
   procedure CPSW_Execute_Rate_Group
        (Requested_Rate : Integer);
   pragma Import(CPP, CPSW_Execute_Rate_Group,
                 "CPSW_Execute_Rate_Group");
   procedure UpdatePersistList
        (Requested_Rate : Float);
   pragma Import(CPP, UpdatePersistList,
                 "UpdatePersistList");
begin
      loop
         begin
            My_Bool := True;
<<Stop_One_Queue_Events>>
            My_Bool := True;
         exception
            when Exception_Occurrence: others => My_Bool := False;
         end;
      end loop;
end Queue1;
