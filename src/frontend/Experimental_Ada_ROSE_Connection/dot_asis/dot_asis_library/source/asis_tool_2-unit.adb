with Asis.Compilation_Units;
with Asis.Elements;
with Asis.Iterator;

-- GNAT-specific:
with Asis.Set_Get;
with A4G.A_Types;

with Asis_Tool_2.Element;
with Dot;

package body Asis_Tool_2.Unit is

   Module_Name : constant String := "Asis_Tool_2.Unit";

   package ACU renames Asis.Compilation_Units;

   -----------------------------------------------------------------------------
   -- Unit_ID support

   function Get_Unit_ID
     (Unit : in Asis.Compilation_Unit)
      return A4G.A_Types.Unit_Id is
   begin
      return Asis.Set_Get.Get_Unit_Id (Unit);
   end Get_Unit_ID;

   function To_Unit_ID
     (ID : in A4G.A_Types.Unit_Id)
      return a_nodes_h.Unit_ID
   is
      (a_nodes_h.Unit_ID (ID));

   function Get_Unit_ID
     (Unit : in Asis.Compilation_Unit)
      return a_nodes_h.Unit_ID is
   begin
      return a_nodes_h.Unit_ID (Asis.Set_Get.Get_Unit_Id (Unit));
   end Get_Unit_ID;

   function To_String
     (ID : in a_nodes_h.Unit_ID)
      return String
   is
     (To_String (ID, Unit_ID_Kind));

   -- END Unit_ID support
   -----------------------------------------------------------------------------

   -- String
   -- Add <Name> = <Value> to the label, and print it if trace is on.
   procedure Add_To_Dot_Label
     (This  : in out Class;
      Name  : in     String;
      Value : in     String) is
   begin
      Add_To_Dot_Label (Dot_Label => This.Dot_Label,
                        Outputs   => This.Outputs,
                        Name      => Name,
                        Value     => Value);
   end;

   -- Wide_String
   -- Add <Name> = <Value> to the label, and print it if trace is on.
   procedure Add_To_Dot_Label
     (This  : in out Class;
      Name  : in     String;
      Value : in     Wide_String) is
   begin
      Add_To_Dot_Label (Dot_Label => This.Dot_Label,
                        Outputs   => This.Outputs,
                        Name      => Name,
                        Value     => To_String (Value));
   end;

   -- Unit_ID
   -- Add <Name> = <Value> to the label, and print it if trace is on.
   procedure Add_To_Dot_Label
     (This  : in out Class;
      Name  : in     String;
      Value : in     a_nodes_h.Unit_ID) is
   begin
      This.Add_To_Dot_Label (Name, To_String (Value));
   end;

   -- Unit_ID
   -- Add <Name> = <Value> to the label, and print it if trace is on.
   procedure Add_To_Dot_Label
     (This  : in out Class;
      Name  : in     String;
      Value : in     Boolean) is
   begin
      Add_To_Dot_Label (Dot_Label => This.Dot_Label,
                        Outputs   => This.Outputs,
                        Name      => Name,
                        Value     => Value);
   end;

   -- Add <Value> to the label in one wide cell, and print it if trace is on.
   procedure Add_To_Dot_Label
     (This  : in out Class;
      Value : in     String) is
   begin
      Add_To_Dot_Label
        (Dot_Label => This.Dot_Label,
         Outputs   => This.Outputs,
         Value     => Value);
   end;

   -- Add an edge for a child element:
   procedure Add_Dot_Edge
     (This  : in out Class;
      From  : in     a_nodes_h.Unit_ID;
      To    : in     a_nodes_h.Element_ID;
      Label : in     String)
   is
   begin
      Add_Dot_Edge (Outputs   => This.Outputs,
                    From      => From,
                    From_Kind => Unit_ID_Kind,
                    To        => To,
                    To_Kind   => Element_ID_Kind,
                    Label     => Label);
   end Add_Dot_Edge;

   -- Add an edge and a dot label for a child element:
   procedure Add_To_Dot_Label_And_Edge
     (This  : in out Class;
      Label : in     String;
      To    : in     a_nodes_h.Element_ID) is
   begin
      -- Element, not Unit:
      This.Add_To_Dot_Label (Label, To_String (To, Element_ID_Kind));
      This.Add_Dot_Edge
        (From      => This.Unit_ID,
         To        => To,
         Label     => Label);
   end Add_To_Dot_Label_And_Edge;

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
            ID : constant a_nodes_h.Unit_ID := Get_Unit_Id (Unit);
            Label : constant String :=
              Dot_Label_Name & " (" & IDs_Index'Image & ")";
         begin
            IDs (IDs_Index) := Interfaces.C.int (ID);
            Add_To_Dot_Label (This, Label, ID);
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
      List_Out       :    out a_nodes_h.Element_ID_List)
   is
      -- For "-"
      use type IC.int;
   begin
      List_Out := Element.To_Element_ID_List
        (Dot_Label       => This.Dot_Label,
         Outputs         => This.Outputs,
         This_Element_ID => This.Unit_ID,
         Elements_In     => Elements_In,
         Dot_Label_Name  => Dot_Label_Name,
         Add_Edges       => True,
         This_Is_Unit    => True);
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


   procedure Process_Compilation_Pragmas
     (This            : in out Class;
      Asis_Unit       : in Asis.Compilation_Unit)
   is
      Compilation_Pragmas : constant Asis.Element_List :=
        Asis.Elements.Compilation_Pragmas (Asis_Unit);
   begin
      for Compilation_Pragma of Compilation_Pragmas loop
         This.Process_Element_Tree (Compilation_Pragma);
      end loop;
   end Process_Compilation_Pragmas;


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
      -- TODO: later
      -- Process_Compilation_Pragmas (This, Asis_Unit);
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
   -- graph.
   procedure Process_Application_Unit
     (This    : in out Class;
      Unit    : in Asis.Compilation_Unit)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name &
        ".Process_Application_Unit";

      Unit_Class     : constant Asis.Unit_Classes := ACU.Unit_Class (Unit);
      Unit_Full_Name : constant Wide_String       := Acu.Unit_Full_Name (Unit);
      Unit_Kind      : constant Asis.Unit_Kinds   := ACU.Unit_Kind (Unit);

      procedure Log (Message : in String) is
      begin
         Put_Line (Module_Name & ":  " & message);
      end;

      -- These are in alphabetical order:
      procedure Add_Can_Be_Main_Program is
         Value : Boolean := ACU.Exists (Unit);
      begin
         This.Add_To_Dot_Label ("Can_Be_Main_Program", Value);
         This.A_Unit.Can_Be_Main_Program := a_nodes_h.Support.To_bool (Value);
      end;

      procedure Add_Compilation_Command_Line_Options is
         WS : constant Wide_String :=
           Acu.Compilation_Command_Line_Options (Unit);
      begin
         This.Add_To_Dot_Label ("Compilation_Command_Line_Options", WS);
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
         ID : constant a_nodes_h.Unit_ID := Get_Unit_Id
           (ACU.Corresponding_Body (Unit));
      begin
         This.Add_To_Dot_Label ("Corresponding_Body", ID);
         This.A_Unit.Corresponding_Body := ID;
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
         ID : constant a_nodes_h.Unit_ID := Get_Unit_Id
           (ACU.Corresponding_Declaration (Unit));
      begin
         This.Add_To_Dot_Label ("Corresponding_Declaration", ID);
         This.A_Unit.Corresponding_Declaration := ID;
      end;

      procedure Add_Corresponding_Parent_Declaration is
         ID : constant a_nodes_h.Unit_ID := Get_Unit_Id
           (ACU.Corresponding_Parent_Declaration (Unit));
      begin
         This.Add_To_Dot_Label ("Corresponding_Parent_Declaration", ID);
         This.A_Unit.Corresponding_Parent_Declaration := ID;
      end;

      procedure Add_Corresponding_Subunit_Parent_Body is
         ID : constant a_nodes_h.Unit_ID := Get_Unit_Id
           (ACU.Corresponding_Subunit_Parent_Body (Unit));
      begin
         This.Add_To_Dot_Label ("Corresponding_Subunit_Parent_Body", ID);
         This.A_Unit.Corresponding_Subunit_Parent_Body := ID;
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
         This.Add_To_Dot_Label ("Exists", Value);
         This.A_Unit.Exists := a_nodes_h.Support.To_bool (Value);
      end;

      procedure Add_Is_Body_Required is
         Value : Boolean := ACU.Exists (Unit);
      begin
         This.Add_To_Dot_Label ("Is_Body_Required", Value);
         This.A_Unit.Is_Body_Required := a_nodes_h.Support.To_bool (Value);
      end;

      procedure Add_Is_Standard is
         Value : Boolean := Asis.Set_Get.Is_Standard (Unit);
      begin
         This.Add_To_Dot_Label ("Is_Standard", Value);
         This.A_Unit.Is_Standard := a_nodes_h.Support.To_bool (Value);
      end;

      procedure Add_Object_Form is
         WS : constant Wide_String := ACU.Object_Form (Unit);
      begin
         -- Empty:
         This.Add_To_Dot_Label ("Object_Form", WS);
         This.A_Unit.Object_Form := To_Chars_Ptr (WS);
      end;

      procedure Add_Object_Name is
         WS : constant Wide_String := ACU.Object_Name (Unit);
      begin
         -- Empty:
         This.Add_To_Dot_Label ("Object_Name", WS);
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
         This.Add_To_Dot_Label ("Text_Form", WS);
         This.A_Unit.Text_Form := To_Chars_Ptr (WS);
      end;

      procedure Add_Text_Name is
         WS : constant Wide_String := ACU.Text_Name (Unit);
      begin
         This.Add_To_Dot_Label ("Text_Name", WS);
         This.A_Unit.Text_Name := To_Chars_Ptr (WS);
      end;

      procedure Add_Unique_Name is
         WS : constant Wide_String := ACU.Unique_Name (Unit);
      begin
         This.Add_To_Dot_Label ("Unique_Name", WS);
         This.A_Unit.Unique_Name := To_Chars_Ptr (WS);
      end;

      procedure Add_Unit_Class is
      begin
         This.Add_To_Dot_Label ("Unit_Class", Unit_Class'Image);
         This.A_Unit.Unit_Class := anhS.To_Unit_Classes (Unit_Class);
      end;

      procedure Add_Unit_Declaration is
         Asis_Element : constant Asis.Element :=
           Asis.Elements.Unit_Declaration (Unit);
         anhS_Element_ID : constant a_nodes_h.Element_ID :=
           Element.Get_Element_ID (Asis_Element);
      begin
         This.Add_To_Dot_Label_And_Edge
           (Label => "Unit_Declaration",
            To    => anhS_Element_ID);
         This.A_Unit.Unit_Declaration := anhS_Element_ID;
      end;

      procedure Add_Unit_Full_Name is
         WS : constant Wide_String := ACU.Unit_Full_Name (Unit);
      begin
         This.Add_To_Dot_Label ("Unit_Full_Name", WS);
         This.A_Unit.Unit_Full_Name := To_Chars_Ptr (WS);
      end;

      procedure Add_Unit_Id is
      begin
         -- ID is in the Dot node twice (once in the Label and once in
         -- Node_ID), but not in the a_node twice.
         This.Add_To_Dot_Label (To_String (This.Unit_ID));
         This.A_Unit.id := This.Unit_ID;
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
         Log ("Processing " & To_String (Unit_Full_Name) & " " &
                To_String (To_Wide_String (Unit_Class)));

            -- Set defaults:
         This.A_Unit := a_nodes_h.Support.Default_Unit_Struct;
                  This.Outputs.Text.End_Line;
         -- Unit ID comes out on next line via Add_Unit_ID:
         This.Outputs.Text.Put_Indented_Line (String'("BEGIN "));
         This.Outputs.Text.Indent;
         This.Dot_Node := Default_Node;
         This.Dot_Label := Default_Label;

         -- Get ID:
         This.Unit_ID := Get_Unit_ID (Unit);
         This.Dot_Node.Node_ID.ID :=
           To_Dot_ID_Type (This.Unit_ID, Unit_ID_Kind);

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
         -- TODO: later
--           Add_Compilation_Pragmas;
         Add_Is_Standard;
      end;

      procedure Finish_Output is
      begin
         This.Dot_Node.Add_Label (This.Dot_Label);

         This.Outputs.Graph.Append_Stmt
           (new Dot.Node_Stmt.Class'(This.Dot_Node));

         This.Outputs.A_Nodes.Push (This.A_Unit);

         This.Outputs.Text.End_Line;
         This.Outputs.Text.Dedent;
         This.Outputs.Text.Put_Indented_Line
           (String'("END " & To_String (This.Unit_ID)));
      end;

      use all type Asis.Unit_Kinds;
   begin -- Process_Application_Unit
      If Unit_Kind /= Not_A_Unit then
         Start_Output;
      end if;

      -- Not common items, in Asis order:
      case Unit_Kind is
         when Not_A_Unit =>
            raise Program_Error with
            Module_Name & " called with: " & Unit_Kind'Image;
         when A_Procedure |
              A_Function |
              A_Generic_Procedure |
              A_Generic_Function =>
            Add_Corresponding_Parent_Declaration;
            Add_Corresponding_Body;
         when A_Package |
              A_Generic_Package =>
            Add_Corresponding_Children;
            Add_Corresponding_Parent_Declaration;
            Add_Corresponding_Body;
         when A_Procedure_Instance |
              A_Function_Instance |
              A_Procedure_Renaming |
              A_Function_Renaming |
              A_Package_Renaming |
              A_Generic_Procedure_Renaming |
              A_Generic_Function_Renaming |
              A_Generic_Package_Renaming =>
            Add_Corresponding_Parent_Declaration;
         when A_Package_Instance =>
            Add_Corresponding_Children;
            Add_Corresponding_Parent_Declaration;
         when A_Procedure_Body |
              A_Function_Body |
              A_Package_Body =>
            Add_Corresponding_Declaration;
            Add_Corresponding_Parent_Declaration;
            Add_Subunits;
         when A_Procedure_Body_Subunit |
              A_Function_Body_Subunit |
              A_Package_Body_Subunit |
              A_Task_Body_Subunit |
              A_Protected_Body_Subunit =>
            Add_Subunits;
            Add_Corresponding_Subunit_Parent_Body;
         when A_Nonexistent_Declaration |
              A_Nonexistent_Body |
              A_Configuration_Compilation =>
            null; -- No more info
         when An_Unknown_Unit=>
            Add_Corresponding_Declaration;
            Add_Corresponding_Body;
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
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process";

      Unit_Full_Name : constant Wide_String       := Acu.Unit_Full_Name (Unit);
      Unit_Origin    : constant Asis.Unit_Origins := Acu.Unit_Origin (Unit);

      procedure Log (Message : in Wide_String) is
      begin
         Put_Line (Module_Name & ":  " & To_String (Message));
      end;

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
            Log ("Skipped " & Unit_Full_Name & " (predefined unit)");
         when Asis.An_Implementation_Unit =>
            Log ("Skipped " & Unit_Full_Name & " (implementation-defined unit)");
         when Asis.Not_An_Origin =>
            Log ("Skipped " & Unit_Full_Name & " (non-existent unit)");
      end case;

   end Process;

end Asis_Tool_2.Unit;
