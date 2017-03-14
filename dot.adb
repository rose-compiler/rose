package body Dot is

   ------------
   -- EXPORTED:
   ------------
   procedure Put (Item : in ID_Type) is
   begin
      if Length (Item) > 0 then
         Put_Spaced (To_String (Item));
      end if;
   end Put;

   ------------
   -- EXPORTED:
   ------------
   procedure Put_Spaced (Item : in String) is
   begin
      Put (Item & " ");
   end Put_Spaced;

end Dot;
