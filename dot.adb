package body Dot is

   -- Output support:

   ------------
   -- EXPORTED:
   ------------
   procedure Put_Spaced (Item : in String) is
   begin
      if Item'Length > 0 then
         Put (Item & " ");
      end if;
   end Put_Spaced;

   -- END Output support

   ------------
   -- EXPORTED:
   ------------
   function To_String (Item : in ID_Type)
                       return String is
   begin
      if Length (Item) > 0 then
         return '"' & Inherited_To_String(Item) & '"';
      else
         return "";
      end if;
   end;

   ------------
   -- EXPORTED:
   ------------
   function To_String (Item : in Compass_Pt_Type)
                       return String is
   begin
      case Item is
         when Underscore =>
            return "_";
         when others =>
            return Item'Image;
      end case;
   end;

   package body Graph is

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
         Put_Spaced (To_String(This.ID));
         Put ("{");
         New_Line;
         Put ("}");
         New_Line;
      end print;

   end Graph;

end Dot;
