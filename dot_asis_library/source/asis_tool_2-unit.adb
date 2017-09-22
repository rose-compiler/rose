with Asis.Compilation_Units;
with Asis.Elements;
with Asis.Iterator;

-- GNAT-specific:
with Asis.Set_Get;
with A4G.A_Types;

with Asis_Tool_2.Element;
with Dot;

package body Asis_Tool_2.Unit is

   package ACU renames Asis.Compilation_Units;

   -- Add <Name> = <Value> to the label, and print it if trace is on.
   procedure Add_To_Dot_Label
     (This  : in out Class;
      Name  : in     String;
      Value : in     String) is
   begin
-- Instead of this, put the "attribute" in the label:
--        This.Node.Attr_List.Add_Assign_To_First_Attr
--          (Name  => Name,
--           Value => Value);
      This.Dot_Label.Add_Eq_Row(L => Name, R => Value);
      This.Outputs.Text.Put_Indented_Line (Name & " => " & Value);
   end;


   -- Add <Name> = <Value> to the label, and print it if trace is on.
   procedure Add_To_Dot_Label
     (This  : in out Class;
      Name  : in     String;
      Value : in     Wide_String) is
   begin
      This.Add_To_Dot_Label (Name, To_String (Value));
   end;


   -- Add <Value> to the label in one wide cell, and print it if trace is on.
   procedure Add_To_Dot_Label
     (This  : in out Class;
      Value : in     String) is
   begin
      This.Dot_Label.Add_3_Col_Cell(Value);
      This.Outputs.Text.Put_Indented_Line (Value);
   end;


   procedure Add_Dot_Edge
     (Graph : not null access Dot.Graphs.Class;
      From  : in     A4G.A_Types.Unit_Id;
      To    : in     Types.Node_Id;
      Label : in     String)
   is
      Edge_Stmt : Dot.Edges.Stmts.Class; -- Initialized
   begin
      if not Types."=" (To, Types.Empty) then
         Edge_Stmt.LHS.Node_Id.ID := To_Dot_ID_Type (From);
         Edge_Stmt.RHS.Node_Id.ID := To_Dot_ID_Type (To);
         Edge_Stmt.Attr_List.Add_Assign_To_First_Attr
           (Name  => "label",
            Value => Label);
         Graph.Append_Stmt (new Dot.Edges.Stmts.Class'(Edge_Stmt));
      end if;
   end;


   -- Add an edge and a dot label for a child element:
   procedure Add_To_Dot_Label_And_Edge
     (This  : in out Class;
      Label : in     String;
      To    : in     Types.Node_Id) is
   begin
      This.Add_To_Dot_Label (Label, To_String (To));
      Add_Dot_Edge (Graph => This.Outputs.Graph,
                    From  => This.Unit_ID,
                    To    => To,
                    Label => Label);
   end;


   procedure Add_Unit_List
     (This           : in out class;
      Units_In       : in     Asis.Compilation_Unit_List;
      Dot_Label_Name : in     String;
      List_Out       :    out a_nodes_h.Unit_List)
   is
      Count : constant Natural := Units_In'Length;
      IDs : anhS.Unit_ID_Array_Access := new
        anhS.Unit_ID_Array (1 .. Count);
      IDs_Index : Positive := IDs'First;
   begin
      for Unit of Units_In loop
         declare
            ID : constant A4G.A_Types.Unit_ID := Asis.Set_Get.Get_Unit_Id (Unit);
            Label : constant String :=
              Dot_Label_Name & " (" & IDs_Index'Image & ")";
         begin
            IDs (IDs_Index) := Interfaces.C.int (ID);
            Add_To_Dot_Label (This, Label, To_String (ID));
            IDs_Index := IDs_Index + 1;
         end;
      end loop;
      List_Out :=
        (length => Interfaces.C.int(Count),
         IDs    => anhS.To_Unit_ID_Ptr (IDs));
   end Add_Unit_List;


   procedure Add_Element_List
     (This           : in out class;
      Elements_In    : in     Asis.Element_List;
      Dot_Label_Name : in     String;
      List_Out       :    out a_nodes_h.Element_List)
   is
      Count : constant Natural := Elements_In'Length;
      IDs : anhS.Element_ID_Array_Access := new
        anhS.Element_ID_Array (1 .. Count);
      IDs_Index : Positive := IDs'First;
   begin
      for Element of Elements_In loop
         declare
            ID : constant Types.Node_ID := Asis.Set_Get.Node_Value (Element);
            Label : constant String :=
              Dot_Label_Name & " (" & IDs_Index'Image & ")";
         begin
            IDs (IDs_Index) := Interfaces.C.int (ID);
            Add_To_Dot_Label (This, Label, To_String (ID));
            IDs_Index := IDs_Index + 1;
         end;
      end loop;
      List_Out :=
        (length => Interfaces.C.int(Count),
         IDs    => anhS.To_Element_ID_Ptr (IDs));
   end Add_Element_List;

   -----------
   -- PRIVATE:
   -----------
   procedure Process_Element_Tree
     (This         : in out Class;
      Asis_Element : in     Asis.Element)
   is
      Tool_Element : Element.Class; -- Initialized
   begin
      This.Add_To_Dot_Label_And_Edge
        (Label => "Element",
         To    =>  Asis.Set_Get.Node (Asis_Element));
      Tool_Element.Process_Element_Tree
        (Element => Asis_Element,
         Outputs => This.Outputs);
   end;

   procedure Process_Context_Clauses
     (This            : in out Class;
      Asis_Unit       : in Asis.Compilation_Unit;
      Include_Pragmas : in Boolean := True)
   is
      Context_Clauses : constant Asis.Element_List :=
        Asis.Elements.Context_Clause_Elements
          (Compilation_Unit => Asis_Unit,
           Include_Pragmas  => Include_Pragmas);
   begin
      for Context_Clause of Context_Clauses loop
         This.Process_Element_Tree (Context_Clause);
      end loop;
   end Process_Context_Clauses;


   -- Process all the elements in the compilation unit:
   procedure Process_Element_Trees
     (This               : in out Class;
      Asis_Unit          : in     Asis.Compilation_Unit;
      Do_Context_Clauses : in     Boolean := True;
      Include_Pragmas    : in     Boolean := True)
   is
      Top_Element_Asis  : Asis.Element := Asis.Elements.Unit_Declaration (Asis_Unit);
   begin
      if Do_Context_Clauses then
         Process_Context_Clauses (This, Asis_Unit, Include_Pragmas);
      end if;
      Process_Element_Tree (This, Top_Element_Asis);
   exception
      when X : others =>
         Print_Exception_Info (X);
         Awti.Put_Line
           ("EXCEPTION when processing unit " &
              Acu.Unit_Full_Name (Asis_Unit));
         raise;
   end Process_Element_Trees;


   function To_Wide_String (this : in Asis.Unit_Classes) return Wide_String is
   begin
      case this is
         when Asis.A_Public_Declaration |
              Asis.A_Private_Declaration =>
            return "spec";
         when Asis.A_Public_Body              |
              Asis.A_Public_Declaration_And_Body |
              Asis.A_Private_Body =>
            return "body";
         when Asis.A_Separate_Body =>
            return "subunit";
         when others =>
            return Asis.Unit_Classes'Wide_Image(this);
      end case;
   end To_Wide_String;


   -- Create a Dot node for this unit, add all the attributes, and append it to the
   -- graph.  Do the A_Unit and the A_Node at the same time:
   procedure Process_Application_Unit
     (This    : in out Class;
      Unit    : in Asis.Compilation_Unit)
   is
      A_Node         : a_nodes_h.Node_Struct      := anhS.Default_Node_Struct;
      Unit_Class     : constant Asis.Unit_Classes := ACU.Unit_Class (Unit);
      Unit_Full_Name : constant Wide_String       := Acu.Unit_Full_Name (Unit);
      Unit_Kind      : constant Asis.Unit_Kinds   := ACU.Unit_Kind (Unit);

      procedure Log (Message : in String) is
      begin
         Put_Line ("Asis_Tool_2.Unit.Process_Application_Unit:  " & message);
      end;

      -- These are in alphabetical order:
      procedure Add_Can_Be_Main_Program is
         Value : Boolean := ACU.Exists (Unit);
      begin
         This.Add_To_Dot_Label ("Can_Be_Main_Program", Value'Image);
         This.A_Unit.Can_Be_Main_Program := a_nodes_h.Support.To_bool (Value);
      end;

      procedure Add_Compilation_Command_Line_Options is
         WS : constant Wide_String :=
           Acu.Compilation_Command_Line_Options (Unit);
      begin
         This.Add_To_Dot_Label ("Compilation_Command_Line_Options", To_String (WS));
         This.A_Unit.Compilation_Command_Line_Options := To_Chars_Ptr (WS);
      end;

      procedure Add_Compilation_Pragmas is
      begin
         Add_Element_List
           (This           => This,
            Elements_In    => Asis.Elements.Compilation_Pragmas (Unit),
            Dot_Label_Name => "Compilation_Pragmas",
            List_Out       => This.A_Unit.Compilation_Pragmas);
      end;

      procedure Add_Context_Clause_Elements is
      begin
         Add_Element_List
           (This           => This,
            Elements_In    => Asis.Elements.Context_Clause_Elements (Unit),
            Dot_Label_Name => "Context_Clause_Elements",
            List_Out       => This.A_Unit.Context_Clause_Elements);
      end;

      procedure Add_Corresponding_Body is
         ID : constant A4G.A_Types.Unit_Id := Asis.Set_Get.Get_Unit_Id
           (ACU.Corresponding_Body (Unit));
      begin
         This.Add_To_Dot_Label ("Corresponding_Body", To_String (ID));
         This.A_Unit.Corresponding_Body := a_nodes_h.Node_ID (ID);
      end;

      procedure Add_Corresponding_Children is
      begin
         Add_Unit_List
           (This           => This,
            Units_In       => ACU.Corresponding_Children (Unit),
            Dot_Label_Name => "Corresponding_Children",
            List_Out       => This.A_Unit.Corresponding_Children);
      end;

      procedure Add_Corresponding_Declaration is
         ID : constant A4G.A_Types.Unit_Id := Asis.Set_Get.Get_Unit_Id
           (ACU.Corresponding_Declaration (Unit));
      begin
         This.Add_To_Dot_Label ("Corresponding_Declaration", To_String (ID));
         This.A_Unit.Corresponding_Declaration := a_nodes_h.Node_ID (ID);
      end;

      procedure Add_Corresponding_Parent_Declaration is
         ID : constant A4G.A_Types.Unit_Id := Asis.Set_Get.Get_Unit_Id
           (ACU.Corresponding_Parent_Declaration (Unit));
      begin
         This.Add_To_Dot_Label ("Corresponding_Parent_Declaration", To_String (ID));
         This.A_Unit.Corresponding_Parent_Declaration := a_nodes_h.Node_ID (ID);
      end;

      procedure Add_Corresponding_Subunit_Parent_Body is
         ID : constant A4G.A_Types.Unit_Id := Asis.Set_Get.Get_Unit_Id
           (ACU.Corresponding_Subunit_Parent_Body (Unit));
      begin
         This.Add_To_Dot_Label ("Corresponding_Subunit_Parent_Body", To_String (ID));
         This.A_Unit.Corresponding_Subunit_Parent_Body := a_nodes_h.Node_ID (ID);
      end;

      procedure Add_Debug_Image is
         WS : constant Wide_String := ACU.Debug_Image (Unit);
      begin
         This.A_Unit.Debug_Image := To_Chars_Ptr (WS);
      end;

      procedure Add_Enclosing_Container is
--           Container : constant Asis.Ada_Environments.Containers.Container :=
--             ACU.Enclosing_Container (Unit);
      begin
         null; -- TODO: Finish if needed:
--           This.A_Unit.Enclosing_Container := Container;
      end;

      procedure Add_Enclosing_Context is
        Context : constant Asis.Context := ACU.Enclosing_Context (Unit);
      begin
         null; -- TODO: Finish if needed:
--           This.A_Unit.Enclosing_Context := Context;
      end;

      procedure Add_Exists is
         Value : Boolean := ACU.Exists (Unit);
      begin
         This.Add_To_Dot_Label ("Exists", Value'Image);
         This.A_Unit.Exists := a_nodes_h.Support.To_bool (Value);
      end;

      procedure Add_Is_Body_Required is
         Value : Boolean := ACU.Exists (Unit);
      begin
         This.Add_To_Dot_Label ("Is_Body_Required", Value'Image);
         This.A_Unit.Is_Body_Required := a_nodes_h.Support.To_bool (Value);
      end;

      procedure Add_Object_Form is
         WS : constant Wide_String := ACU.Object_Form (Unit);
      begin
         -- Empty:
         This.Add_To_Dot_Label ("Object_Form", To_String (WS));
         This.A_Unit.Object_Form := To_Chars_Ptr (WS);
      end;

      procedure Add_Object_Name is
         WS : constant Wide_String := ACU.Object_Name (Unit);
      begin
         -- Empty:
         This.Add_To_Dot_Label ("Object_Name", To_String (WS));
         This.A_Unit.Object_Name := To_Chars_Ptr (WS);
      end;

      procedure Add_Subunits is
      begin
         Add_Unit_List
           (This           => This,
            Units_In       => ACU.Subunits (Unit),
            Dot_Label_Name => "Subunits",
            List_Out       => This.A_Unit.Subunits);
      end;

      procedure Add_Text_Form is
         WS : constant Wide_String := ACU.Text_Form (Unit);
      begin
         -- Empty:
         This.Add_To_Dot_Label ("Text_Form", To_String (WS));
         This.A_Unit.Text_Form := To_Chars_Ptr (WS);
      end;

      procedure Add_Text_Name is
         WS : constant Wide_String := ACU.Text_Name (Unit);
      begin
         This.Add_To_Dot_Label ("Text_Name", To_String (WS));
         This.A_Unit.Text_Name := To_Chars_Ptr (WS);
      end;

      procedure Add_Unique_Name is
         WS : constant Wide_String := ACU.Unique_Name (Unit);
      begin
         This.Add_To_Dot_Label ("Unique_Name", To_String (WS));
         This.A_Unit.Unique_Name := To_Chars_Ptr (WS);
      end;

      procedure Add_Unit_Class is
      begin
         This.Add_To_Dot_Label ("Unit_Class", Unit_Class'Image);
         This.A_Unit.Unit_Class := anhS.To_Unit_Classes (Unit_Class);
      end;

      procedure Add_Unit_Declaration is
         ID : constant Types.Node_Id :=
           Asis.Set_Get.Node (Asis.Elements.Unit_Declaration (Unit));
      begin
         This.Add_To_Dot_Label ("Unit_Declaration", To_String (ID));
         This.A_Unit.Unit_Declaration := a_nodes_h.Node_ID (ID);
      end;

      procedure Add_Unit_Full_Name is
         WS : constant Wide_String := ACU.Unit_Full_Name (Unit);
      begin
         This.Add_To_Dot_Label ("Unit_Full_Name", To_String (WS));
         This.A_Unit.Unit_Full_Name := To_Chars_Ptr (WS);
      end;

      procedure Add_Unit_Id is
         ID : constant A4G.A_Types.Unit_Id := Asis.Set_Get.Get_Unit_Id (Unit);
      begin
         This.Unit_ID := ID;
         This.Dot_Node.Node_ID.ID := To_Dot_ID_Type (ID);
         This.A_Unit.id := Interfaces.C.int(ID);
         This.Add_To_Dot_Label(To_String (ID));
      end;

      procedure Add_Unit_Kind is
      begin
         This.Add_To_Dot_Label ("Unit_Kind", Unit_Kind'Image);
         This.A_Unit.Unit_Kind := anhS.To_Unit_Kinds (Unit_Kind);
      end;

      procedure Add_Unit_Origin is
         Unit_Origin : constant Asis.Unit_Origins := ACU.Unit_Origin (Unit);
      begin
         This.Add_To_Dot_Label ("Unit_Origin", Unit_Origin'Image);
         This.A_Unit.Unit_Origin := anhS.To_Unit_Origins (Unit_Origin);
      end;

      procedure Start_Output is
         Default_Node  : Dot.Node_Stmt.Class; -- Initialized
         Default_Label : Dot.HTML_Like_Labels.Class; -- Initialized
      begin
         This.Outputs.Text.Indent;
         This.Outputs.Text.End_Line;
         This.Dot_Node := Default_Node;
         This.Dot_Label := Default_Label;
         This.A_Unit := a_nodes_h.Support.Default_Unit_Struct;

         -- Want these at the top:
         Add_Unit_Id;
         Add_Unit_Kind;

         -- Common items, in Asis order:
         Add_Unit_Class;
         Add_Unit_Origin;
         if not Asis.Compilation_Units.Is_Nil (Unit) then
            Add_Enclosing_Context;
            Add_Enclosing_Container;
         end if;
         Add_Unit_Full_Name;
         Add_Unique_Name;
         Add_Exists;
         Add_Can_Be_Main_Program;
         Add_Is_Body_Required;
         Add_Text_Name;
         Add_Text_Form;
         Add_Object_Name;
         Add_Object_Form;
         Add_Compilation_Command_Line_Options;
         Add_Debug_Image;
         Add_Unit_Declaration;
         Add_Context_Clause_Elements;
         Add_Compilation_Pragmas;
      end;

      procedure Finish_Output is
         A_Node : a_nodes_h.Node_Struct := anhS.Default_Node_Struct;
      begin
         This.Dot_Node.Add_Label (This.Dot_Label);

         This.Outputs.Graph.Append_Stmt
           (new Dot.Node_Stmt.Class'(This.Dot_Node));

         A_Node.Node_Kind := a_nodes_h.A_Unit_Node;
         A_Node.The_Union.Unit := This.A_Unit;
         This.Outputs.A_Nodes.Push (A_Node);

         This.Outputs.Text.End_Line;
         This.Outputs.Text.Dedent;
      end;

      use all type Asis.Unit_Kinds;
   begin -- Process_Application_Unit
      If Unit_Kind /= Not_A_Unit then
         Log ("Processing " & To_String (Unit_Full_Name) & " " &
                To_String (To_Wide_String (Unit_Class)));
         Start_Output;
      end if;

      -- Not common items, in Asis order:
      case Unit_Kind is
         when Not_A_Unit =>
               raise Program_Error with
                 "Unit.Add_Output called with: " & Unit_Kind'Image;
         when A_Procedure =>
            Add_Corresponding_Parent_Declaration;
            Add_Corresponding_Body;
         when A_Function =>
            Add_Corresponding_Parent_Declaration;
            Add_Corresponding_Body;
         when A_Package =>
            Add_Corresponding_Children;
            Add_Corresponding_Parent_Declaration;
            Add_Corresponding_Body;
         when A_Generic_Procedure =>
            Add_Corresponding_Parent_Declaration;
            Add_Corresponding_Body;
         when A_Generic_Function =>
            Add_Corresponding_Parent_Declaration;
            Add_Corresponding_Body;
         when A_Generic_Package =>
            Add_Corresponding_Children;
            Add_Corresponding_Parent_Declaration;
            Add_Corresponding_Body;
         when A_Procedure_Instance =>
            Add_Corresponding_Parent_Declaration;
         when A_Function_Instance =>
            Add_Corresponding_Parent_Declaration;
         when A_Package_Instance =>
            Add_Corresponding_Children;
            Add_Corresponding_Parent_Declaration;
         when A_Procedure_Renaming =>
            Add_Corresponding_Parent_Declaration;
         when A_Function_Renaming =>
            Add_Corresponding_Parent_Declaration;
         when A_Package_Renaming =>
            Add_Corresponding_Parent_Declaration;
         when A_Generic_Procedure_Renaming =>
            Add_Corresponding_Parent_Declaration;
         when A_Generic_Function_Renaming =>
            Add_Corresponding_Parent_Declaration;
         when A_Generic_Package_Renaming =>
            Add_Corresponding_Parent_Declaration;
         when A_Procedure_Body =>
            Add_Corresponding_Declaration;
            Add_Corresponding_Parent_Declaration;
            Add_Subunits;
         when A_Function_Body =>
            Add_Corresponding_Declaration;
            Add_Corresponding_Parent_Declaration;
            Add_Subunits;
         when A_Package_Body =>
            Add_Corresponding_Declaration;
            Add_Corresponding_Parent_Declaration;
            Add_Subunits;
         when A_Procedure_Body_Subunit =>
            Add_Subunits;
            Add_Corresponding_Subunit_Parent_Body;
         when A_Function_Body_Subunit =>
            Add_Subunits;
            Add_Corresponding_Subunit_Parent_Body;
         when A_Package_Body_Subunit =>
            Add_Subunits;
            Add_Corresponding_Subunit_Parent_Body;
         when A_Task_Body_Subunit =>
            Add_Subunits;
            Add_Corresponding_Subunit_Parent_Body;
         when A_Protected_Body_Subunit =>
            Add_Subunits;
            Add_Corresponding_Subunit_Parent_Body;
         when A_Nonexistent_Declaration =>
            null; -- No more info
         when A_Nonexistent_Body =>
            null; -- No more info
         when A_Configuration_Compilation =>
            null; -- No more info
         when An_Unknown_Unit=>
            Add_Corresponding_Declaration;
      end case;

      Finish_Output;
      Process_Element_Trees (This, Unit);
      Log ("DONE Processing " & To_String (Unit_Full_Name) & " " &
             To_String (To_Wide_String (Unit_Class)));
   end Process_Application_Unit;

   ------------
   -- EXPORTED:
   ------------
   procedure Process
     (This    : in out Class;
      Unit    : in     Asis.Compilation_Unit;
      Outputs : in     Outputs_Record)
   is
      Unit_Full_Name : constant Wide_String       := Acu.Unit_Full_Name (Unit);
      Unit_Origin    : constant Asis.Unit_Origins := Acu.Unit_Origin (Unit);
   begin
      -- I would like to just pass Outputs through and not store it in the
      -- object, since it is all pointers and we doesn't need to store their
      -- values between calls to Process_Element_Tree. Outputs has to go into
      -- Add_To_Dot_Label, though, so  we'll put it in the object and pass
      -- that:
      This.Outputs := Outputs;

      case Unit_Origin is
         when Asis.An_Application_Unit =>
            Process_Application_Unit (This, Unit);
         when Asis.A_Predefined_Unit =>
            Trace_Put_Line ("Skipped " & Unit_Full_Name & " (predefined unit)");
         when Asis.An_Implementation_Unit =>
            Trace_Put_Line ("Skipped " & Unit_Full_Name & " (implementation-defined unit)");
         when Asis.Not_An_Origin =>
            Trace_Put_Line ("Skipped " & Unit_Full_Name & " (non-existent unit)");
      end case;

   end Process;

end Asis_Tool_2.Unit;
