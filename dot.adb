--with Ada.Strings.Mapping;
with Ada.Characters.Handling;

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

   function Case_Insensitive_Equals (L, R : in String)
                                     return Boolean is
   begin
      -- Prevents recursion in case this function is named "=":
      return Standard."=" (Ada.Characters.Handling.To_Lower (L),
                           Ada.Characters.Handling.To_Lower (R));
   end Case_Insensitive_Equals;


   function Is_Reserved_Word (Item : in String)
                              return boolean
   is
      function "=" (L, R : in String)
                    return Boolean
                    renames Case_Insensitive_Equals;
   begin
      return
        Item = "node" or else
        Item = "edge" or else
        Item = "graph" or else
        Item = "digraph" or else
        Item = "subgraph" or else
        Item = "strict";
   end Is_Reserved_Word;

   ------------
   -- EXPORTED:
   ------------
   function To_String (Item : in ID_Type)
                       return String is
      Item_String : constant String :=
        ASU.To_String (ASU.Unbounded_String(Item));
   begin
      if Is_Reserved_Word (Item_String) then
         return '"' & Item_String & '"';
      else
         return Item_String;
      end if;
   end To_String;

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
   end To_String;

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
