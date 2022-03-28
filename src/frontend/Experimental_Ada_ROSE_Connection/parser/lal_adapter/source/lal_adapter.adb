with Ada.Characters.Conversions;

package body Lal_Adapter is

   package Acc renames Ada.Characters.Conversions;

   ------------
   -- EXPORTED:
   ------------
   procedure Initialize
     (This : in out Output_Accesses_Record) is
   begin
      This.Text := new Indented_Text.Class;
      This.Graph := Dot.Graphs.Create (Is_Digraph => True,
                                               Is_Strict  => False);
      This.A_Nodes := new A_Nodes.Class;
   end Initialize;

   -----------
   -- PRIVATE:
   -----------
   function To_String (Item : in Wide_String) return String is
     (Acc.To_String (Item, Substitute => ' '));

   -----------
   -- PRIVATE:
   -----------
   function To_String (Item : in Wide_Wide_String) return String is
     (Acc.To_String (Item, Substitute => ' '));

   -----------
   -- PRIVATE:
   -----------
   function To_Quoted_String (Item : in String) return String is
      ('"' & Item & '"');

   -----------
   -- PRIVATE:
   -----------
   function To_Chars_Ptr (Item : in Wide_String) return Interfaces.C.Strings.chars_ptr is
     (Interfaces.C.Strings.New_String (To_String(Item)));

   -----------
   -- PRIVATE:
   -----------
   function Spaceless_Image (Item : in Natural) return String is
      Leading_Space_Image : constant String := Item'Image;
   begin
      return Leading_Space_Image (2 .. Leading_Space_Image'Last);
   end;

   -----------
   -- PRIVATE:
   -----------
   function To_String
     (Id   : in IC.int;
      Kind : in ID_Kind)
      return String
   is
      function Add_Prefix_To (Item : in String) return String is
      begin
         case Kind is
            when Unit_ID_Kind =>
               return "Unit_" & Item;
            when Element_ID_Kind =>
               return "LAL_Node_" & Item;
         end case;
      end Add_Prefix_To;

      use type IC.int;
   begin
      if anhS.Is_Empty (ID) then
         return "(none)";
      elsif not anhS.Is_Valid (ID) then
         return "***Invalid ID***";
      else
         return Add_Prefix_To (Spaceless_Image (Natural (Id)));
      end if;
   exception
      when Constraint_Error =>
         raise Program_Error with "Id =>" & Id'Image & ", Kind => " & Kind'Image;
   end To_String;

   -----------
   -- PRIVATE:
   -----------
   function To_Dot_ID_Type
     (Id   : in IC.int;
      Kind : in ID_Kind)
      return Dot.ID_Type is
     (Dot.To_ID_Type (To_String (Id, Kind)));

   -----------
   -- PRIVATE:
   -----------
   procedure Add_To_Dot_Label
     (This      : in     Class;
      Dot_Label : in out Dot.HTML_Like_Labels.Class;
      Value     : in     String) is
   begin
      Dot_Label.Add_3_Col_Cell(Value);
      This.Outputs.Text.Put_Indented_Line (Value);
   end;

   -----------
   -- PRIVATE:
   -----------
   procedure Add_To_Dot_Label
     (This      : in     Class;
      Dot_Label : in out Dot.HTML_Like_Labels.Class;
      Name      : in     String;
      Value     : in     String) is
   begin
      Dot_Label.Add_Eq_Row (L => Name, R => Value);
      This.Outputs.Text.Put_Indented_Line (Name & " => " & Value);
   end;

   -----------
   -- PRIVATE:
   -----------
   procedure Add_To_Dot_Label
     (This      : in     Class;
      Dot_Label : in out Dot.HTML_Like_Labels.Class;
      Name      : in     String;
      Value     : in     Boolean) is
   begin
      if Value then
         declare
            Value_String : constant String := Value'Image;
         begin
            Dot_Label.Add_Eq_Row (L => Name, R => Value_String);
            This.Outputs.Text.Put_Indented_Line (Name & " => " & Value_String);
         end;
      end if;
   end;

   -----------
   -- PRIVATE:
   -----------
   procedure Add_Dot_Edge
     (This      : in     Class;
      From      : in     IC.int;
      From_Kind : in     ID_Kind;
      To        : in     IC.int;
      To_Kind   : in     ID_Kind;
      Label     : in     String)
   is
      Edge_Stmt : Dot.Edges.Stmts.Class; -- Initialized
   begin
      if not anhS.Is_Empty (To) then
         Edge_Stmt.LHS.Node_Id.ID := To_Dot_ID_Type (From, From_Kind);
         Edge_Stmt.RHS.Node_Id.ID := To_Dot_ID_Type (To, To_Kind);
         Edge_Stmt.Attr_List.Add_Assign_To_First_Attr
           (Name  => "label",
            Value => Label);
         This.Outputs.Graph.Append_Stmt (new Dot.Edges.Stmts.Class'(Edge_Stmt));
      end if;
   end Add_Dot_Edge;

end Lal_Adapter;
