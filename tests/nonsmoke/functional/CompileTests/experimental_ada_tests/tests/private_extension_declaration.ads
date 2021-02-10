package private_extension_declaration is

   type Expression is tagged null record;
   type PrivateType is new Expression with private; 
private
   type PrivateType is new Expression with 
       record
         The_Variable : Integer;
       end record;
 

end private_extension_declaration;
