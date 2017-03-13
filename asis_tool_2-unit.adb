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

   function To_String (this : in Asis.Unit_Classes) return Wide_String is
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
   end To_String;


   ------------
   -- EXPORTED:
   ------------
   procedure Process (Object : in out Class) is
      Unit_Name   : constant Wide_String       := Asis.Compilation_Units.Unit_Full_Name (Object.My_Unit);
      Unit_Origin : constant Asis.Unit_Origins := Asis.Compilation_Units.Unit_Origin (Object.My_Unit);
      Unit_Class  : constant Asis.Unit_Classes := Asis.Compilation_Units.Unit_Class (Object.My_Unit);
      use Ada.Wide_Text_IO;
   begin
      case Unit_Origin is
         when Asis.An_Application_Unit =>
            New_Line;
            Put_Line ("Processing: " & Unit_Name & " (" & To_String (Unit_Class) & ")");

            -- Do actual work:
            Object.Process_Element_Trees;

            Trace_Put_Line ("Done");
         when Asis.A_Predefined_Unit =>
            Trace_Put_Line ("Skipped " & Unit_Name & " (predefined unit)");
         when Asis.An_Implementation_Unit =>
            Trace_Put_Line ("Skipped " & Unit_Name & " (implementation-defined unit)");
         when Asis.Not_An_Origin =>
            Trace_Put_Line ("Skipped " & Unit_Name & " (non-existent unit)");
      end case;
   end Process;


end Asis_Tool_2.Unit;
