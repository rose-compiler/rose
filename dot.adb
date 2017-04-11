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

   -- NOT TASK SAFE:
   Is_Digraph : Boolean := True;

   package body Stmt is

      ------------
      -- EXPORTED
      ------------
      procedure Put
        (These : in List_Of_Access_All_Class)
      is
         First_Item : Boolean := True;
      begin
         Indented.Indent;
         for Item of These loop
            if First_Item Then
               First_Item := False;
            else
               Indented.Put (";");
            end if;
            Indented.End_Line_If_Needed;
            Item.Put;
         end loop;
         Indented.Dedent;
      end Put;

   end Stmt;

   package body Assign is

      ------------
      -- EXPORTED
      ------------
      procedure Put
        (This : in Class) is
      begin
         Put (This.L);
         Indented.Put ("=");
         Put (This.R);
      end Put;

      ------------
      -- EXPORTED
      ------------
      procedure Put
        (These : in List_Of_Class)
      is
         First_Item : Boolean := True;
      begin
         Indented.Indent;
         for Item of These loop
            if First_Item Then
               First_Item := False;
            else
               Indented.Put (",");
            end if;
            Indented.End_Line_If_Needed;
            Item.Put;
         end loop;
         Indented.Dedent;
      end Put;

      procedure Append
        (These : in out List_Of_Class;
         L, R  : in     String) is
      begin
         These.Append
           ((L => To_ID_Type (L),
             R => To_ID_Type (R)));
      end Append;

   end Assign;

   package body Attr is

      ------------
      -- EXPORTED
      ------------
      procedure Put
        (This : in Class) is
      begin
         Indented.Indent;
         Indented.Put ("[");
         This.A_List.Put;
         Indented.Put (" ]");
         Indented.Dedent;
     end Put;

      ------------
      -- EXPORTED
      ------------
      procedure Put
        (These : in List_Of_Class)
      is
      begin
         Indented.Indent;
         for Item of These loop
            Indented.End_Line_If_Needed;
            Item.Put;
         end loop;
         Indented.Dedent;
      end Put;

      ------------
      -- EXPORTED
      ------------
      procedure Add_Assign_To_First_Attr
        (Attr_List : in out List_Of_Class;
         Name  : in     String;
         Value : in     String)
      is
         procedure Add_Assign (Attr : in out Dot.Attr.Class) is
         begin
            Attr.A_List.Append (Name, Value);
         end Add_Assign;
      begin
         if Attr_List.Is_Empty then
            Attr_List.Append (Dot.Attr.Null_Class);
         end if;
         Attr_List.Update_Element
           (Position => Dot.Attr.First(Attr_List),
            Process  => Add_Assign'Access);
      end Add_Assign_To_First_Attr;

   end Attr;

   package body Attr_Stmt is

      ------------
      -- EXPORTED
      ------------
      procedure Put
        (This     : in Class) is
      begin
         Indented.Put ("<attr stmt>");
         Indented.End_Line_If_Needed;
      end Put;

      ------------
      -- EXPORTED
      ------------
      procedure Append_To
        (This      : in Class;
         Stmt_List : in out Stmt.List_Of_Access_All_Class) is
      begin
         Stmt_List.Append (new Class'(This));
      end Append_To;

   end Attr_Stmt;

   package body Node_ID is

      ------------
      -- EXPORTED
      ------------
      procedure Put (This : in Port_Class) is
      begin
         if This.Has_ID then
            Indented.Put (":");
            Put (This.ID);
         end if;
         if This.Has_Compass_Pt then
            Indented.Put (":");
            Indented.Put (To_String (This.Compass_Pt));
         end if;
      end Put;

      ------------
      -- EXPORTED
      ------------
      procedure Put (This : in Class) is
      begin
         Put (This.ID);
         This.Port.Put;
      end Put;

   end Node_ID;

   package body Node_Stmt is

      ------------
      -- EXPORTED
      ------------
      procedure Put
        (This : in Class) is
      begin
         This.Node_Id.Put;
         This.Attr_List.Put;
      end Put;

      ------------
      -- EXPORTED
      ------------
      procedure Append_To
        (This      : in Class;
         Stmt_List : in out Stmt.List_Of_Access_All_Class) is
      begin
         Stmt_List.Append (new Class'(This));
      end Append_To;

   end Node_Stmt;

   package body Subgraphs is

      procedure Put
        (This : in Class) is
      begin
         if Length (This.ID) > 0 then
            Indented.Put ("subgraph ");
            Put (This.ID);
         end if;
         This.Stmt_List.Put;
      end Put;

   end Subgraphs;

   package body Edges is

      procedure Put_Edgeop is
      begin
         if Is_Digraph then
            Indented.Put (" -> ");
         else
            Indented.Put (" -- ");
         end if;
      end Put_Edgeop;

      ------------
      -- EXPORTED
      ------------
      package body Terminals is

         ------------
         -- EXPORTED
         ------------
         procedure Put
           (This : in Class) is
         begin
            case This.Kind is
               when Node_Kind =>
                  This.Node_Id.Put;
               when Subgraph_Kind =>
                  This.Subgraph.Put;
            end case;
         end Put;


         ------------
         -- EXPORTED
         ------------
         procedure Put
           (These : in List_Of_Class) is
         begin
            for This of These loop
               Put_Edgeop;
               Put(This);
               end loop;
         end Put;

      end Terminals;

      ------------
      -- EXPORTED
      ------------
      package body Stmts is

         ------------
         -- EXPORTED
         ------------
         procedure Put
           (This : in Class) is
         begin
            Terminals.Put (This.LHS);
            Put_Edgeop;
            Terminals.Put (This.RHS);
            This.RHSs.Put;
            This.Attr_List.Put;
         end Put;

         ------------
         -- EXPORTED
         ------------
         procedure Append_To
           (This      : in Class;
            Stmt_List : in out Stmt.List_Of_Access_All_Class) is
         begin
            Stmt_List.Append (new Class'(This));
         end Append_To;

      end Stmts;

   end Edges;

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
      procedure Put
        (This : access Class) is
      begin
         if This.Strict then
            Indented.Put ("strict ");
         end if;
         if This.Digraph then
            Indented.Put ("digraph ");
            Is_Digraph := True;
         else
            Indented.Put ("graph ");
            Is_Digraph := False;
         end if;
         Indented.Put_Spaced (To_String(This.ID));
         Indented.Put ("{");
         This.Stmt_List.Put;
         Indented.New_Line;
         Indented.Put ("}");
      end Put;

   end Graphs;

   -----------
   -- PRIVATE:
   -----------
   package body Indented is

      Indent_Level : Natural := 0;
      Indent_Size  : constant Natural := 2;

      function Current_Indent_Col return ATI.Positive_Count is
        (ATI.Positive_Count((Indent_Level * Indent_Size) + 1));

      -- If the indent is increased in the middle of a line, this will ensure
      -- that the next put is at that indent or better:
      procedure Put_Indent is
         use type ATI.Positive_Count;
      begin
         if ATI.Col < Current_Indent_Col then
            ATI.Set_Col (Current_Indent_Col);
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
      procedure New_Line is
      begin
         ATI.New_Line;
      end New_Line;

      ------------
      -- EXPORTED
      ------------
      procedure End_Line_If_Needed is
         use type ATI.Positive_Count;
      begin
         if ATI.Col > Current_Indent_Col then
            New_Line;
         end if;
      end End_Line_If_Needed;

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
   procedure Put (This : in ID_Type) is
   begin
      Indented.Put (To_String(This));
   end Put;

end Dot;
