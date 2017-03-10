with Ada.Wide_Text_IO;

with Asis;
with Asis.Compilation_Units;
with Asis.Elements;
with Asis.Iterator;

with Asis_Tool_2.Element;

package body Asis_Tool_2.Unit is


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
         Asis_Tool_2.Element.Process_Element_Tree (Context_Clause);
      end loop;
   end Process_Context_Clauses;


   -- Process all the elements in the compilation unit:
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
      Asis_Tool_2.Element.Process_Element_Tree (Top_Element);
   exception
      when X : others =>
         Print_Exception_Info (X);
         Put_Line
           ("EXCEPTION when processing unit " & Asis.Compilation_Units.Unit_Full_Name (Object.My_Unit));
         raise;
   end Process_Element_Trees;

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


end Asis_Tool_2.Unit;
