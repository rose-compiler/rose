
--~ with Report;
with Ada.IO_Exceptions; 

procedure Exception_RenamingRenaming(b : IN boolean) is
  Name_Error : exception renames Ada.IO_Exceptions.Name_Error;
  Name_Exception: exception renames Name_Error;
begin
  if b then 
    raise Name_Exception;
  end if;
exception
  when Name_Error =>
    null;
end Exception_RenamingRenaming;
