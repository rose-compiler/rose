with Ada.Wide_Text_IO;

with Asis;
with Asis.Compilation_Units;
with Asis.Elements;
with Asis.Iterator;

with Asis_Tool_2.Element;
with Dot;

package body Asis_Tool_2.Unit is

   package Acu renames Asis.Compilation_Units;


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
         declare
            Tool_Element : Element.Class; -- Initialized
         begin
            Tool_Element.Process_Element_Tree
              (Element => Context_Clause,
               Graph   => This.Graph);
         end;
      end loop;
   end Process_Context_Clauses;


   -- Process all the elements in the compilation unit:
   procedure Process_Element_Trees
     (This               : in out Class;
      Do_Context_Clauses : in     Boolean := True;
      Include_Pragmas    : in     Boolean := True)
   is
      Top_Element  : Asis.Element := Asis.Elements.Unit_Declaration (This.Asis_Unit);
      Tool_Element : Element.Class; -- Initialized
      use Ada.Wide_Text_IO;
   begin
      if Do_Context_Clauses then
         This.Process_Context_Clauses
           (Asis_Unit       => This.Asis_Unit,
            Include_Pragmas => Include_Pragmas);
      end if;
      Tool_Element.Process_Element_Tree
        (Element => Top_Element,
         Graph   => This.Graph);
   exception
      when X : others =>
         Print_Exception_Info (X);
         Put_Line
           ("EXCEPTION when processing unit " &
              Acu.Unit_Full_Name (This.Asis_Unit));
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
      Node        : Dot.Node_Stmt.Class; -- Initialized
      As          : Dot.Assign.List; -- Initialized
      Unique_Name : constant String := To_String (Acu.Unique_Name (Unit));
   begin
      Node.Node_ID.ID := Dot.To_ID_Type (Unique_Name);
      -- These are in alphabetical order:
      As.Append ("Compilation_Command_Line_Options",
                 To_String (Acu.Compilation_Command_Line_Options (Unit)));
      -- Empty:
      --           As.Append ("This_Form", To_String (Acu.Object_Form (Unit)));
      -- Empty:
      --           As.Append ("Object_Name", To_String (Acu.Object_Name (Unit)));
      -- Empty:
      --           As.Append ("Text_Form", To_String (Acu.Text_Form (Unit)));
      As.Append ("Text_Name",      To_String (Acu.Text_Name (Unit)));
      As.Append ("Unique_Name",    Unique_Name);
      As.Append ("Unit_Class",     Acu.Unit_Class (Unit)'Image);
      As.Append ("Unit_Full_Name", To_String (Acu.Unit_Full_Name (Unit)));
      As.Append ("Unit_Kind",      Acu.Unit_Kind (Unit)'Image);
      As.Append ("Unit_Origin",    Acu.Unit_Origin (Unit)'Image);
      Node.Attrs.Append (As);
      Graph.Append_Stmt (new Dot.Node_Stmt.Class'(Node));
   end Add_To_Graph;

   ------------
   -- EXPORTED:
   ------------
   procedure Process
     (This      : in out Class;
      Asis_Unit : in     Asis.Compilation_Unit;
      Graph     : in     Dot.Graphs.Access_Class)
   is
   begin
      This.Asis_Unit := Asis_Unit;
      This.Graph := Graph;
      declare
         Unit_Full_Name : constant Wide_String       := Acu.Unit_Full_Name (This.Asis_Unit);
         Unit_Origin    : constant Asis.Unit_Origins := Acu.Unit_Origin (This.Asis_Unit);
      begin
         case Unit_Origin is
         when Asis.An_Application_Unit =>
            Awti.New_Line;
            Awti.Put_Line
              ("Processing " & Acu.Unit_Full_Name (This.Asis_Unit) & " " &
                 To_Wide_String (Acu.Unit_Class (This.Asis_Unit)));

            -- Do actual work:
            Add_To_Graph (This.Asis_Unit, This.Graph);
            This.Process_Element_Trees;

            Awti.Put_Line
              ("DONE processing " & Unit_Full_Name & " " &
                 To_Wide_String (Acu.Unit_Class (This.Asis_Unit)));
         when Asis.A_Predefined_Unit =>
            Trace_Put_Line ("Skipped " & Unit_Full_Name & " (predefined unit)");
         when Asis.An_Implementation_Unit =>
            Trace_Put_Line ("Skipped " & Unit_Full_Name & " (implementation-defined unit)");
         when Asis.Not_An_Origin =>
            Trace_Put_Line ("Skipped " & Unit_Full_Name & " (non-existent unit)");
         end case;
      end;
   end Process;


end Asis_Tool_2.Unit;
