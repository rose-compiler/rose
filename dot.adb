with Ada.Characters.Handling;
with Ada.Strings.Fixed;

package body Dot is

   ------------
   -- EXPORTED
   ------------
   function To_ID_Type (Item : in Wide_String) return ID_Type is
   begin
      return To_ID_Type (Ada.Characters.Handling.To_String(Item));
   end To_ID_Type;

   package body Graphs is

      ------------
      -- EXPORTED
      ------------
      procedure Set_Is_Digraph
        (This : access Class;
         To   : in     Boolean) is
      begin
         This.Digraph := To;
      end;

      ------------
      -- EXPORTED
      ------------
      procedure Set_Is_Strict
        (This : access Class;
         To   : in     Boolean) is
      begin
         This.Strict := To;
      end;

      ------------
      -- EXPORTED
      ------------
      procedure Set_ID
        (This : access Class;
         To   : in     String) is
      begin
         This.ID := To_ID_Type (To);
      end;

      ------------
      -- EXPORTED
      ------------
      procedure Append_Stmt
        (This     : access Class;
         The_Stmt : in     Stmt.Access_All_Class) is
      begin
         This.Stmt_List.Append (The_Stmt);
      end;

      ------------
      -- EXPORTED
      ------------
      function Stmt_Count
        (This : access Class)
         return Natural is
      begin
         return Natural (This.Stmt_List.Length);
      end;

      ------------
      -- EXPORTED
      ------------
      procedure Print (This : access Class) is
      begin
         if This.Strict then
            Indented.Put ("strict ");
         end if;
         if This.Digraph then
            Indented.Put ("digraph ");
         else
            Indented.Put ("graph ");
         end if;
         Indented.Put_Spaced (To_String(This.ID));
         This.Stmt_List.Print;
      end print;

   end Graphs;

   package body Stmt is

      ------------
      -- EXPORTED
      ------------
      procedure Print (This_List : in List) is
      begin
         Indented.Put_Line ("{");
         Indented.Indent;
         for This of This_List loop
            This.Print;
         end loop;
         Indented.Dedent;
         Indented.Put_Line ("}");
      end Print;

   end Stmt;

   package body Assign is

      ------------
      -- EXPORTED
      ------------
      procedure Print (This : in Class) is begin
--           Indented.Put_Spaced ("<assign>");
         Print (This.L);
         Indented.Put (" = ");
         Print (This.R);
      end Print;

      ------------
      -- EXPORTED
      ------------
      procedure Print (This : in List) is
         First_Item : Boolean := True;
      begin
         --           Indented.Put_Spaced ("<assign list>");
         Indented.Indent;
         for Item of This loop
            if First_Item then
               First_Item := False;
            else
               Indented.Put_Line (",");
            end if;
            Item.Print;
         end loop;
         Indented.Dedent;
      end Print;

      procedure Append
        (Container : in out List;
         L, R      : in     String) is
      begin
         Container.Append
           ((L => To_ID_Type (L),
             R => To_ID_Type (R)));
      end Append;

   end Assign;

   package body Attr is

      -- There is no Print for Attr because it is nothing more than an Assign List.

      ------------
      -- EXPORTED
      ------------
      procedure Print (This : in List) is
      begin
         --           Indented.Put_Spaced ("<attr list>");
         Indented.New_Line_If_Needed;
         for Item of This loop
            Indented.Indent;
            Indented.Put ("[");
            Item.Print;
            Indented.Put_Line (" ]");
            Indented.Dedent;
         end loop;
      end Print;

   end Attr;

   package body Attr_Stmt is

      ------------
      -- EXPORTED
      ------------
      procedure Print (This : in Class) is
      begin
         Indented.Put_Line ("<attr stmt>");
      end Print;

      ------------
      -- EXPORTED
      ------------
      procedure Append_To
        (This      : in Class;
         Stmt_List : in out Stmt.List) is
      begin
         Stmt_List.Append (new Class'(This));
      end Append_To;

   end Attr_Stmt;

   package body Node_ID is

      ------------
      -- EXPORTED
      ------------
      procedure Print (This : in Port_Class) is
      begin
         if This.Has_ID then
            Indented.Put (":");
            Print (This.ID);
         end if;
         if This.Has_Compass_Pt then
            Indented.Put (":");
            Indented.Put (To_String (This.Compass_Pt));
         end if;
      end Print;

      ------------
      -- EXPORTED
      ------------
      procedure Print (This : in Class) is
      begin
         Print (This.ID);
         This.Port.Print;
      end Print;

   end Node_ID;

   package body Node_Stmt is

      ------------
      -- EXPORTED
      ------------
      procedure Print (This : in Class) is
      begin
--           Indented.Put_Spaced ("<node>");
         This.Node_Id.Print;
         Indented.Put(" ");
         This.Attrs.Print;
         Indented.New_Line_If_Needed;
      end Print;

      ------------
      -- EXPORTED
      ------------
      procedure Append_To
        (This      : in Class;
         Stmt_List : in out Stmt.List) is
      begin
         Stmt_List.Append (new Class'(This));
      end Append_To;

   end Node_Stmt;

   package body Edge_Stmt is

      ------------
      -- EXPORTED
      ------------
      procedure Print (This : in Class) is
      begin
         Indented.Put_Line ("<edge stmt>");
      end Print;

      ------------
      -- EXPORTED
      ------------
      procedure Append_To
        (This      : in Class;
         Stmt_List : in out Stmt.List) is
      begin
         Stmt_List.Append (new Class'(This));
      end Append_To;

   end Edge_Stmt;

   -----------
   -- PRIVATE:
   -----------
   package body Indented is

      Indent_Level  : Natural := 0;
      Indent_Size   : constant Natural := 2;

      -- If the indent is increased in the middle of a line, this will ensure
      -- that the next put is at that indent or better:
      procedure Put_Indent is
         Minimum_Col : constant ATI.Positive_Count :=
           ATI.Positive_Count ((Indent_Level * Indent_Size) + 1);
         use type ATI.Positive_Count;
      begin
         if ATI.Col < Minimum_Col then
            ATI.Set_Col (Minimum_Col);
         end if;
      end Put_Indent;

      ------------
      -- EXPORTED
      ------------
      procedure Indent is
      begin
         Indent_Level := Indent_Level + 1;
      end Indent;

      ------------
      -- EXPORTED
      ------------
      procedure Dedent is
      begin
         Indent_Level := Indent_Level - 1;
      end Dedent;

      ------------
      -- EXPORTED
      ------------
      procedure Put (Item : in String) is
      begin
         Put_Indent;
         ATI.Put (Item);
      end Put;

      ------------
      -- EXPORTED
      ------------
      procedure Put_Line (Item : in String) is
      begin
         Put_Indent;
         ATI.Put_Line (Item);
      end Put_Line;

      ------------
      -- EXPORTED
      ------------
      procedure New_Line is
      begin
         ATI.New_Line;
      end New_Line;

      ------------
      -- EXPORTED
      ------------
      procedure New_Line_If_Needed is
         use type ATI.Positive_Count;
      begin
         if ATI.Col > 1 then
            New_Line;
         end if;
      end New_Line_If_Needed;

      ------------
      -- EXPORTED
      ------------
      procedure Put_Spaced (Item : in String) is
      begin
         if Item'Length > 0 then
            Put (Item & " ");
         end if;
      end Put_Spaced;

   end Indented;

   function Case_Insensitive_Equals (L, R : in String)
                                     return Boolean is
   begin
      -- Prevents recursion in case this function is named "=":
      return Standard."=" (Ada.Characters.Handling.To_Lower (L),
                           Ada.Characters.Handling.To_Lower (R));
   end Case_Insensitive_Equals;


   function Is_Reserved_Word (Item : in String)
                              return boolean is
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

   function Contains_Space (Item : in String)
                            return boolean is
   begin
      return Ada.Strings.Fixed.Index (Item, " ") > 0;
   end Contains_Space;

   ------------
   -- PRIVATE:
   ------------
   function To_String (Item : in ID_Type)
                       return String is
      Item_String : constant String :=
        ASU.To_String (ASU.Unbounded_String(Item));
   begin
      if Item_String'Length = 0 then
         return """""";
      elsif Is_Reserved_Word (Item_String) or else
        Contains_Space (Item_String) then
         return '"' & Item_String & '"';
      else
         return Item_String;
      end if;
   end To_String;

   ------------
   -- PRIVATE:
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

   ------------
   -- PRIVATE:
   ------------
   procedure Print (This : in ID_Type) is
   begin
      Indented.Put (To_String(This));
   end Print;

end Dot;
