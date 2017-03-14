package body Dot.Graph is

   ------------
   -- EXPORTED
   ------------
   procedure Print (This : in Class) is
   begin
      if This.Strict then
         Put_Spaced ("strict");
      end if;
      if This.Digraph then
         Put_Spaced ("digraph");
      else
         Put_Spaced ("graph");
      end if;
      Put (This.ID);
      Put ("{");
      New_Line;
      Put ("}");
      New_Line;
   end print;

end Dot.Graph;
