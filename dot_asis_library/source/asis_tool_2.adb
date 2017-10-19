with Ada.Characters.Handling;

--GNAT-specific:
with Asis.Set_Get;
with GNAT.Traceback.Symbolic;

package body Asis_Tool_2 is

   package Ach renames Ada.Characters.Handling;

   -----------
   -- PRIVATE:
   -----------
   function To_String (Item : in Wide_String) return String is
     (Ach.To_String (Item, Substitute => ' '));

   -----------
   -- PRIVATE:
   -----------
   function To_Quoted_String (Item : in Wide_String) return String is
      ('"' & To_String (Item) & '"');

   -----------
   -- PRIVATE:
   -----------
   function To_Wide_String (Item : in String) return Wide_String
     renames Ach.To_Wide_String;

   -----------
   -- PRIVATE:
   -----------
   function To_Chars_Ptr (Item : in Wide_String) return Interfaces.C.Strings.chars_ptr is
     (Interfaces.C.Strings.New_String (To_String(Item)));

   -----------
   -- PRIVATE:
   -----------
   procedure Print_Exception_Info (X : in Ada.Exceptions.Exception_Occurrence) is
   begin
      Awti.Put_Line ("EXCEPTION: " & To_Wide_String (Aex.Exception_Name (X)) &
                       " (" & To_Wide_String (Aex.Exception_Information (X)) & ")");
      Awti.Put_Line ("TRACEBACK: ");
      Awti.Put_Line (To_Wide_String (GNAT.Traceback.Symbolic.Symbolic_Traceback (X)));
   end Print_Exception_Info;

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
               return "Element_" & Item;
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

   ------------
   -- EXPORTED:
   ------------
   procedure Add_To_Dot_Label
     (Dot_Label : in out Dot.HTML_Like_Labels.Class;
      Outputs   : in out Outputs_Record;
      Name      : in     String;
      Value     : in     String) is
   begin
      Dot_Label.Add_Eq_Row (L => Name, R => Value);
      Outputs.Text.Put_Indented_Line (Name & " => " & Value);
   end;

   ------------
   -- EXPORTED:
   ------------
   procedure Add_To_Dot_Label
     (Dot_Label : in out Dot.HTML_Like_Labels.Class;
      Outputs   : in out Outputs_Record;
      Name      : in     String;
      Value     : in     Boolean) is
   begin
      if Value then
         declare
            Value_String : constant String := Value'Image;
         begin
            Dot_Label.Add_Eq_Row (L => Name, R => Value_String);
            Outputs.Text.Put_Indented_Line (Name & " => " & Value_String);
         end;
      end if;
   end;

   ------------
   -- EXPORTED:
   ------------
   procedure Add_To_Dot_Label
     (Dot_Label : in out Dot.HTML_Like_Labels.Class;
      Outputs   : in out Outputs_Record;
      Value     : in     String) is
   begin
      Dot_Label.Add_3_Col_Cell(Value);
      Outputs.Text.Put_Indented_Line (Value);
   end;

   ------------
   -- EXPORTED:
   ------------
   procedure Add_Dot_Edge
     (Outputs   : in out Outputs_Record;
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
         Outputs.Graph.Append_Stmt (new Dot.Edges.Stmts.Class'(Edge_Stmt));
      end if;
   end Add_Dot_Edge;

end Asis_Tool_2;
