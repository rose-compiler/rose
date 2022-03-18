PROCEDURE subobj_renaming_declaration IS
   soureVal : String := "ABCDEFGHIJKLMNOPQRSTUVXYZ";
   renamedVal : String renames soureVal(3..6); 
BEGIN
   NULL;
END;
