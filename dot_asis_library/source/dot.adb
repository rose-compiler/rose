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
        (These : in List_Of_Access_All_Class;
         File  : in ATI.File_Type)
      is
         First_Item : Boolean := True;
      begin
         Indented.Indent;
         for Item of These loop
            if First_Item Then
               First_Item := False;
            else
               Indented.Put (File, ";");
            end if;
            Indented.End_Line_If_Needed (File);
            Item.Put (File);
         end loop;
         Indented.Dedent;
      end Put;

   end Stmt;

   package body Assign is

      ------------
      -- EXPORTED
      ------------
      procedure Put
        (This : in Class;
         File : in ATI.File_Type) is
      begin
         Put (This.L, File);
         Indented.Put (File, "=");
         Put (This.R, File);
      end Put;

      ------------
      -- EXPORTED
      ------------
      procedure Put
        (These : in List_Of_Class;
         File  : in ATI.File_Type)
      is
         First_Item : Boolean := True;
      begin
         Indented.Indent;
         for Item of These loop
            if First_Item Then
               First_Item := False;
            else
               Indented.Put (File, ",");
            end if;
            Indented.End_Line_If_Needed (File);
            Item.Put (File);
         end loop;
         Indented.Dedent;
      end Put;

      ------------
      -- EXPORTED
      ------------
      procedure Append
        (These : in out List_Of_Class;
         L, R  : in     String) is
      begin
         These.Append
           ((L => To_ID_Type (L),
             R => To_ID_Type (R)));
      end Append;

      ------------unction Empty_List return List_Of_Class
      -- EXPORTED
      ------------
      function Empty_List return List_Of_Class is
      begin
         return List_Of_Class'(Lists.Empty_List with null record);
      end Empty_List;

   end Assign;

   package body Attr is

      ------------
      -- EXPORTED
      ------------
      procedure Put
        (This : in Class;
         File : in ATI.File_Type) is
      begin
         Indented.Indent;
         Indented.Put (File, "[");
         This.A_List.Put (File);
         Indented.Put (File, " ]");
         Indented.Dedent;
     end Put;

      ------------
      -- EXPORTED
      ------------
      procedure Put
        (These : in List_Of_Class;
         File  : in ATI.File_Type)
      is
      begin
         Indented.Indent;
         for Item of These loop
            Indented.End_Line_If_Needed (File);
            Item.Put (File);
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
        (This : in Class;
         File : in ATI.File_Type) is
      begin
         case This.Kind is
            when Graph =>
               Indented.Put (File, "graph");
            when Node =>
               Indented.Put (File, "node");
            when Edge =>
               Indented.Put (File, "edge");
         end case;
         This.Attr_List.Put (File);
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
      procedure Put
        (This : in Port_Class;
         File : in ATI.File_Type) is
      begin
         if This.Has_ID then
            Indented.Put (File, ":");
            Put (This.ID, File);
         end if;
         if This.Has_Compass_Pt then
            Indented.Put (File, ":");
            Indented.Put (File, To_String (This.Compass_Pt));
         end if;
      end Put;

      ------------
      -- EXPORTED
      ------------
      procedure Put
        (This : in Class;
         File : in ATI.File_Type) is
      begin
         Put (This.ID, File);
         This.Port.Put (File);
      end Put;

   end Node_ID;

   package body Node_Stmt is

      ------------
      -- EXPORTED:
      ------------
      procedure Put
        (This : in Class;
         File : in ATI.File_Type) is
      begin
         This.Node_Id.Put (File);
         This.Attr_List.Put (File);
      end Put;

      ------------
      -- EXPORTED:
      ------------
      procedure Append_To
        (This      : in Class;
         Stmt_List : in out Stmt.List_Of_Access_All_Class) is
      begin
         Stmt_List.Append (new Class'(This));
      end Append_To;

      ------------
      -- EXPORTED:
      ------------
      procedure Add_Label
        (This     : in out Class;
         HL_Label : HTML_Like_Labels.Class) is
      begin
         This.Attr_List.Add_Assign_To_First_Attr ("label", HL_Label.To_String);
      end Add_Label;


   end Node_Stmt;

   package body HTML_Like_Labels is

      NL : constant String := (1 => ASCII.LF);

      ------------
      -- EXPORTED:
      ------------
      procedure Add_Eq_Row
        (This : in out Class;
         L, R : in String) is
      begin
         This.Rows.Append
           ((1 => To_Unbounded_String (L),
             2 => To_Unbounded_String (R)));
      end Add_Eq_Row;

      ------------
      -- EXPORTED:c
      ------------
      procedure Add_3_Col_Cell
        (This : in out Class;
         Text : in String) is
      begin
         -- To_Image omits the "=" when it sees this pattern:
         This.Rows.Append
           ((1 => To_Unbounded_String (""),
             2 => To_Unbounded_String (Text)));
      end Add_3_Col_Cell;

      function To_Left_TD
        (LR : in Unbounded_String)
         return Unbounded_String is
      begin
         return "<TD ALIGN=""LEFT"">" & LR & "</TD>";
      end To_Left_TD;

      function To_Center_TD
        (LR : in String)
         return Unbounded_String is
      begin
         return "<TD>" & To_Unbounded_String (LR) & "</TD>";
      end To_Center_TD;


      function To_Center_3_TD
        (LR : in Unbounded_String)
         return Unbounded_String is
      begin
         return "<TD COLSPAN=""3"">" & LR & "</TD>";
      end To_Center_3_TD;


      function To_Center_3_TD
        (LR : in String)
         return Unbounded_String is
      begin
         return To_Center_3_TD (To_Unbounded_String (LR));
      end To_Center_3_TD;


      function To_TR (This : in LR_Pair) return Unbounded_String is
         -- In some dot file viewers, e.g. zgrviewer, the longest content
         -- in a left cell overlaps the center cell. The extra spaces below
         -- attempt to address this:
         function Pad (Item : in Unbounded_String) return Unbounded_String is
         begin
            return Item & (1 .. (Length (Item) / 4) + 1=> ' ');
         end Pad;
      begin
         if This (1) = "" then
            return "          <TR>" &
              To_Center_3_TD (This (2)) & "</TR>" & NL;
         else
            return "          <TR>" &
              To_Left_TD (Pad (This (1))) & To_Center_TD (" = ") &
              To_Left_TD (This (2)) & "</TR>" & NL;
            end if;
      end To_TR;



      function To_Unbounded_String (This : in Class) return Unbounded_String is
      begin
         return
           "<<TABLE BORDER=""0"" CELLBORDER=""0"" CELLSPACING=""0"" CELLPADDING=""0""> " & NL &
           To_Unbounded_String (This.Rows) &
           "          </TABLE>>";
      end To_Unbounded_String;

      ------------
      -- EXPORTED:
      ------------
      function To_String (This : in Class) return String is begin
         return To_String (To_Unbounded_String (This));
      end To_String;

      ------------
      -- EXPORTED:
      ------------
      function To_Unbounded_String (This : in LR_Pair_List) return Unbounded_String is
         Result : Unbounded_String; -- Initialized
      begin
         for Pair of This loop
            Result := Result & To_TR (Pair);
         end loop;
         return Result;
      end To_Unbounded_String;

   end HTML_Like_Labels;

   package body Subgraphs is

      procedure Put
        (This : in Class;
         File : in ATI.File_Type) is
      begin
         if Length (This.ID) > 0 then
            Indented.Put (File, "subgraph ");
            Put (This.ID, File);
         end if;
         This.Stmt_List.Put (File);
      end Put;

   end Subgraphs;

   package body Edges is

      procedure Put_Edgeop
        (File : in ATI.File_Type) is
      begin
         if Is_Digraph then
            Indented.Put (File, " -> ");
         else
            Indented.Put (File, " -- ");
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
           (This : in Class;
            File : in ATI.File_Type) is
         begin
            case This.Kind is
               when Node_Kind =>
                  This.Node_Id.Put (File);
               when Subgraph_Kind =>
                  This.Subgraph.Put (File);
            end case;
         end Put;

         ------------
         -- EXPORTED
         ------------
         procedure Put
           (These : in List_Of_Class;
            File  : in ATI.File_Type) is
         begin
            for This of These loop
               Put_Edgeop (File);
               -- Why doesn't this compile?
               -- This.Put (File);
               Put(This, File);
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
           (This : in Class;
            File : in ATI.File_Type) is
         begin
            Terminals.Put (This.LHS, File);
            Put_Edgeop (File);
            Terminals.Put (This.RHS, File);
            This.RHSs.Put (File);
            This.Attr_List.Put (File);
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
      function Create
        (Is_Digraph : in Boolean;
         Is_Strict  : in Boolean)
         return Access_Class
      is
         Result : Access_Class;
      begin
         Result := new Dot.Graphs.Class;
         Result.Set_Is_Digraph (Is_Digraph);
         Result.Set_Is_Strict (Is_Strict);
         return Result;
      end Create;

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
        (This : access Class;
         File : in     ATI.File_Type) is
      begin
         if This.Strict then
            Indented.Put (File, "strict ");
         end if;
         if This.Digraph then
            Indented.Put (File, "digraph ");
            Is_Digraph := True;
         else
            Indented.Put (File, "graph ");
            Is_Digraph := False;
         end if;
         Indented.Put_Spaced (File, To_String(This.ID));
         Indented.Put (File, "{");
         This.Stmt_List.Put (File);
         Indented.New_Line (File);
         Indented.Put (File, "}");
      end Put;

      ------------
      -- EXPORTED
      ------------
      procedure Write_File
        (This      : access Class;
         Name      : in     String;
         Overwrite : in     Boolean := False)
      is
         Output_File : ATI.File_Type;
         procedure Log (Message : in String) is
         begin
            ATI.Put_Line ("Dot.Graphs.Write_File:  " & Message);
         end;
         procedure Create is
            File_Name   : constant String := Name & ".dot";
         begin
--              if Overwrite then
--                 -- Delete the file if it already exists:
--                 begin
--                    ATI.Open (File => Output_File,
--                              Mode => ATI.In_File,
--                              Name => File_Name);
--                    -- Only if there is such a file:
--                    ATI.Delete (Output_File);
--                 exception
--                    when ATI.Name_Error =>
--                       ATI.Put_Line ("Got Name_Error trying to open """ & File_Name & """");
--                       null;
--                 end;
--              end if;
            begin
               Log ("Creating """ & File_Name & """");
               ATI.Create (File => Output_File,
                           Mode => ATI.Out_File,
                           Name => File_Name);
            exception
               when ATI.Use_Error =>
                  raise Usage_Error with
                    "Could not create file """ & File_Name & """.";
            end;
         end Create;
      begin
         Create;
         This.Put (Output_File);
         ATI.Close (Output_File);
      end Write_File;

   end Graphs;

   -----------
   -- PRIVATE:
   -----------
   package body Indented is

      Indent_Size  : constant Natural := 2;
      Indent_Level : Natural := 0;

      function Current_Indent_Col return ATI.Positive_Count is
        (ATI.Positive_Count((Indent_Level * Indent_Size) + 1));

      -----------
      -- PRIVATE:
      -----------
      procedure Put_Indent (File : in ATI.File_Type) is
         use type ATI.Positive_Count;
      begin
         if ATI.Col (File) < Current_Indent_Col then
            ATI.Set_Col (File, Current_Indent_Col);
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
      procedure Put
        (File : in ATI.File_Type;
         Item : in String) is
      begin
         Put_Indent (File);
         ATI.Put (File, Item);
      end Put;

      ------------
      -- EXPORTED
      ------------
      procedure New_Line (File : in ATI.File_Type) is
      begin
         ATI.New_Line (File);
      end New_Line;

      ------------
      -- EXPORTED
      ------------
      procedure End_Line_If_Needed  (File : in ATI.File_Type) is
         use type ATI.Positive_Count;
      begin
         if ATI.Col (File) > Current_Indent_Col then
            New_Line (File);
         end if;
      end End_Line_If_Needed;

      ------------
      -- EXPORTED
      ------------
      procedure Put_Spaced
        (File : in ATI.File_Type;
         Item : in String) is
      begin
         if Item'Length > 0 then
            Put (File, Item & " ");
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

   function Is_Html_Like (Item : in String)
                          return boolean is
   begin
      return Item (Item'First) = '<';
   end Is_Html_Like;

   ------------
   -- PRIVATE:
   ------------
   function To_String (Item : in ID_Type)
                       return String is
      Item_String : constant String :=
        ASU.To_String (ASU.Unbounded_String(Item));
      function Quoted_Item_String return String is
      begin
         return '"' & Item_String & '"';
      end Quoted_Item_String;
   begin
      if Item_String'Length = 0 then
         return """""";
      elsif Is_Reserved_Word (Item_String) then
         return Quoted_Item_String;
      elsif Is_Html_Like (Item_String) then
         return Item_String;
      elsif Contains_Space (Item_String) then
         return Quoted_Item_String;
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
   procedure Put
     (This : in ID_Type;
      File : in ATI.File_Type) is
   begin
      Indented.Put (File, To_String(This));
   end Put;

end Dot;
