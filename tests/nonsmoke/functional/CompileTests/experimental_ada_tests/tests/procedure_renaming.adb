procedure Procedure_Renaming is

   procedure Do_It is
   begin
      null;
   end;
    
   procedure Do_That renames Do_It;

begin
   Do_That;
end Procedure_Renaming;
