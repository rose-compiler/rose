with Ada.Wide_Text_IO;

with Asis;
with Asis.Compilation_Units;
with Asis.Elements;
with Asis.Iterator;

-- Does actual work:
with Actuals_For_Traversing;

package body Asis_Tool_2.Unit is

   -- Process all the elements in the compilation unit:
   procedure Process_Element_Trees
     (Object             : in out Class;
      Do_Context_Clauses : in     Boolean := True;
      Include_Pragmas    : in     Boolean := True);

   ------------
   -- EXPORTED:
   ------------
   procedure Set_Up
     (Object   : in out Class;
      Asis_Unit : in     Asis.Compilation_Unit)
   is
   begin
      Object.My_Unit := Asis_Unit;
   end Set_Up;

   ------------
   -- EXPORTED:
   ------------
   procedure Process (Object : in out Class) is
      Unit_Origin : Asis.Unit_Origins     := Asis.Compilation_Units.Unit_Origin (Object.My_Unit);
      Unit_Class  : Asis.Unit_Classes     := Asis.Compilation_Units.Unit_Class (Object.My_Unit);
      use Ada.Wide_Text_IO;
   begin
      if Trace_On then
         Put ("Processing: ");
         Put (Asis.Compilation_Units.Unit_Full_Name (Object.My_Unit));
         case Unit_Class is
            when Asis.A_Public_Declaration |
                 Asis.A_Private_Declaration =>
               Put (" (spec)");
            when Asis.A_Separate_Body =>
               Put (" (subunit)");
            when Asis.A_Public_Body              |
                 Asis.A_Public_Declaration_And_Body |
                 Asis.A_Private_Body                =>
               Put (" (body)");
            when others =>
               Put_Line (" (???)");
         end case;
         New_Line;
      end if;

      case Unit_Origin is
         when Asis.An_Application_Unit =>

            -- Do actual work:
            Object.Process_Element_Trees;

            Trace_Put ("Done");
         when Asis.A_Predefined_Unit =>
            Trace_Put ("Skipped (predefined unit)");
         when Asis.An_Implementation_Unit =>
            Trace_Put ("Skipped (implementation-defined unit)");
         when Asis.Not_An_Origin =>
            Trace_Put ("Skipped (nonexistent unit)");
      end case;

      if Trace_On then
         New_Line;
         New_Line;
      end if;
   end Process;


   procedure Traverse_Element is new
     Asis.Iterator.Traverse_Element
       (State_Information => Actuals_For_Traversing.Traversal_State,
        Pre_Operation     => Actuals_For_Traversing.Pre_Op,
        Post_Operation    => Actuals_For_Traversing.Post_Op);


   -- Process an element and all of its components:
   procedure Process_Element_Tree (The_Element : Asis.Element) is
      Process_Control : Asis.Traverse_Control := Asis.Continue;
      Process_State   : Actuals_For_Traversing.Traversal_State :=
        Actuals_For_Traversing.Initial_Traversal_State;
   begin
      Traverse_Element
        (Element => The_Element,
         Control => Process_Control,
         State   => Process_State);
   end Process_Element_Tree;


   procedure Process_Context_Clauses
     (Asis_Unit       : in Asis.Compilation_Unit;
      Include_Pragmas : in Boolean := True)
   is
      Context_Clauses : constant Asis.Element_List :=
        Asis.Elements.Context_Clause_Elements
          (Compilation_Unit => Asis_Unit,
           Include_Pragmas  => Include_Pragmas);
   begin
      for Context_Clause of Context_Clauses loop
         Process_Element_Tree (Context_Clause);
      end loop;
   end Process_Context_Clauses;


   procedure Process_Element_Trees
     (Object             : in out Class;
      Do_Context_Clauses : in     Boolean := True;
      Include_Pragmas    : in     Boolean := True)
   is
      Top_Element : Asis.Element := Asis.Elements.Unit_Declaration (Object.My_Unit);
      use Ada.Wide_Text_IO;
   begin
      if Do_Context_Clauses then
         Process_Context_Clauses
           (Asis_Unit       => Object.My_Unit,
            Include_Pragmas => Include_Pragmas);
      end if;
      Process_Element_Tree (Top_Element);
   exception
      when X : others =>
         Print_Exception_Info (X);
         Put_Line
           ("EXCEPTION when processing unit " & Asis.Compilation_Units.Unit_Full_Name (Object.My_Unit));
         raise;
   end Process_Element_Trees;

end Asis_Tool_2.Unit;
