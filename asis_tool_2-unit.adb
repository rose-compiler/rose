with Ada.Wide_Text_IO;

with Asis;
with Asis.Compilation_Units;
with Asis.Elements;
with Asis.Iterator;
-- GNAT-specific:
with Asis.Set_Get;

with a_nodes_h.Support;
with Asis_Tool_2.Element;
with Dot;

package body Asis_Tool_2.Unit is

   package ACU renames Asis.Compilation_Units;
   package anhS renames a_nodes_h.Support;

   procedure Process_Context_Clauses
     (Asis_Unit       : in Asis.Compilation_Unit;
      Outputs         : in Output_Accesses_Record;
      Include_Pragmas : in Boolean := True)
   is
      Context_Clauses : constant Asis.Element_List :=
        Asis.Elements.Context_Clause_Elements
          (Compilation_Unit => Asis_Unit,
           Include_Pragmas  => Include_Pragmas);
   begin
      for Context_Clause of Context_Clauses loop
         declare
            Tool_Element : Element.Class; -- Initialized
         begin
            Tool_Element.Process_Element_Tree
              (Element => Context_Clause,
               Outputs => Outputs);
         end;
      end loop;
   end Process_Context_Clauses;


   -- Process all the elements in the compilation unit:
   procedure Process_Element_Trees
     (Asis_Unit          : in     Asis.Compilation_Unit;
      Outputs            : in     Output_Accesses_Record;
      Do_Context_Clauses : in     Boolean := True;
      Include_Pragmas    : in     Boolean := True)
   is
      Top_Element_Asis  : Asis.Element := Asis.Elements.Unit_Declaration (Asis_Unit);
      Tool_Element : Element.Class; -- Initialized
      use Ada.Wide_Text_IO;
   begin
      if Do_Context_Clauses then
         Process_Context_Clauses
           (Asis_Unit       => Asis_Unit,
            Outputs         => Outputs,
            Include_Pragmas => Include_Pragmas);
      end if;
      Tool_Element.Process_Element_Tree
        (Element => Top_Element_Asis,
         Outputs => Outputs);
   exception
      when X : others =>
         Print_Exception_Info (X);
         Put_Line
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
              Asis.A_Private_Body                =>
            return "body";
         when Asis.A_Separate_Body =>
            return "subunit";
         when others =>
            return Asis.Unit_Classes'Wide_Image(this);
      end case;
   end To_Wide_String;


   -- Create a node for this unit, add all the attributes, and append it to the
   -- graph:
   procedure Add_To_Graph
     (Unit  : in Asis.Compilation_Unit;
      Graph : in Dot.Graphs.Access_Class)
   is
      Node  : Dot.Node_Stmt.Class; -- Initialized
      Label : Dot.HTML_Like_Labels.Class; -- Initialized
   begin
      Node.Node_ID.ID := To_Dot_ID_Type (Unit);
      Label.Add_3_Col_Cell (Node_Id_Image (Unit));
      -- These are in alphabetical order:
      Label.Add_Eq_Row ("Compilation_Command_Line_Options",
                 To_String (Acu.Compilation_Command_Line_Options (Unit)));
      -- Empty:
      -- Node.Attrs.Add_Assign_To_First_Attr ("This_Form",      To_String (Acu.Object_Form (Unit)));
      -- Empty:
      -- Node.Attrs.Add_Assign_To_First_Attr ("Object_Name",    To_String (Acu.Object_Name (Unit)));
      -- Empty:
      -- Node.Attrs.Add_Assign_To_First_Attr ("Text_Form",      To_String (Acu.Text_Form (Unit)));
      Label.Add_Eq_Row ("Text_Name",      To_String (Acu.Text_Name (Unit)));
      Label.Add_Eq_Row ("Unique_Name",    To_String (Acu.Unique_Name (Unit)));
      Label.Add_Eq_Row ("Unit_Class",     Acu.Unit_Class (Unit)'Image);
      Label.Add_Eq_Row ("Unit_Full_Name", To_String (Acu.Unit_Full_Name (Unit)));
      Label.Add_Eq_Row ("Unit_Kind",      Acu.Unit_Kind (Unit)'Image);
      Label.Add_Eq_Row ("Unit_Origin",    Acu.Unit_Origin (Unit)'Image);
      Node.Add_Label (Label);
      Graph.Append_Stmt (new Dot.Node_Stmt.Class'(Node));
   end Add_To_Graph;

   procedure Create_And_Push_New_A_Node
     (Asis_Unit : in Asis.Compilation_Unit;
      A_Nodes   : in Standard.A_Nodes.Access_Class)
   is
      Unit : a_nodes_h.Unit_Struct :=
        anhS.Unit_Struct_Default;
      Node : a_nodes_h.Node_Struct :=
        anhS.Node_Struct_Default;
   begin
      Unit.id := Interfaces.C.int(Asis.Set_Get.Get_Unit_Id (Asis_Unit));
      Unit.kind := anhS.To_Unit_Kinds (ACU.Unit_Kind (Asis_Unit));
      Unit.the_class := anhS.To_Unit_Classes (ACU.Unit_Class (Asis_Unit));
      Unit.origin := anhS.To_Unit_Origins (ACU.Unit_Origin (Asis_Unit));
      Unit.full_name := To_Chars_Ptr (ACU.Unit_Full_Name (Asis_Unit));
      Unit.unique_name := To_Chars_Ptr (ACU.Unique_Name (Asis_Unit));
      Unit.text_name := To_Chars_Ptr (ACU.Text_Name( Asis_Unit));
      Unit.debug_image := To_Chars_Ptr (ACU.Debug_Image (Asis_Unit));

      Node.kind := a_nodes_h.A_Unit_Node;
      Node.the_union.unit := Unit;
      A_Nodes.Push (Node);
   end;

   ------------
   -- EXPORTED:
   ------------
   procedure Process
     (This      : in out Class;
      Asis_Unit : in     Asis.Compilation_Unit;
      Outputs   : in     Output_Accesses_Record)
   is
      Unit_Origin    : constant Asis.Unit_Origins := Acu.Unit_Origin (Asis_Unit);
      Unit_Full_Name : constant Wide_String       := Acu.Unit_Full_Name (Asis_Unit);
      Unit_Class     : constant Asis.Unit_Classes := Acu.Unit_Class (Asis_Unit);
   begin
      This.Asis_Unit := Asis_Unit;
      case Unit_Origin is
         when Asis.An_Application_Unit =>
            Awti.New_Line;
            Awti.Put_Line
              ("Processing " & Unit_Full_Name & " " &
                 To_Wide_String (Acu.Unit_Class (This.Asis_Unit)));

            -- Do actual work:
            Create_And_Push_New_A_Node (Asis_Unit, Outputs.A_Nodes);
            Add_To_Graph (Asis_Unit, Outputs.Graph);
            Process_Element_Trees (Asis_Unit, Outputs);

            Awti.Put_Line
              ("DONE processing " & Unit_Full_Name & " " &
                 To_Wide_String (Unit_Class));
         when Asis.A_Predefined_Unit =>
            Trace_Put_Line ("Skipped " & Unit_Full_Name & " (predefined unit)");
         when Asis.An_Implementation_Unit =>
            Trace_Put_Line ("Skipped " & Unit_Full_Name & " (implementation-defined unit)");
         when Asis.Not_An_Origin =>
            Trace_Put_Line ("Skipped " & Unit_Full_Name & " (non-existent unit)");
      end case;
   end Process;


end Asis_Tool_2.Unit;
