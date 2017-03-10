with Ada.Exceptions;
with Ada.Wide_Text_IO;
with Asis;
with Asis.Ada_Environments;
with Asis.Compilation_Units;
with Asis.Implementation;
with Asis.Exceptions; use ASIS.Exceptions;
with Asis.Errors;

with Asis_Tool_2.Unit;

package body Asis_Tool_2.Context is

   ------------
   -- EXPORTED:
   ------------
   procedure Process (Object : in out Class) is
   begin
      Asis.Implementation.Initialize;
      Asis.Ada_Environments.Associate
        (The_Context => Object.My_Context,
         Name        => Asis.Ada_Environments.Default_Name);
      Asis.Ada_Environments.Open (Object.My_Context);
      Object.Process_Units;
      Asis.Ada_Environments.Close (Object.My_Context);
      Asis.Ada_Environments.Dissociate (Object.My_Context);
      Asis.Implementation.Finalize;
   end Process;

   -----------
   -- PRIVATE:
   -----------
   procedure Process_Units (Object : in out Class) is
      use Ada.Wide_Text_IO;
      use Asis.Exceptions;
     Asis_Units : Asis.Compilation_Unit_List :=
        Asis.Compilation_Units.Compilation_Units (Object.My_Context);
   begin
      for Asis_Unit of Asis_Units loop
         declare
            Tool_Unit : Asis_Tool_2.Unit.Class;
         begin
            Tool_Unit.Set_Up (Asis_Unit);
            Tool_Unit.Process;
         end;
      end loop;
   exception
      when Ex : ASIS_Inappropriate_Context          |
           ASIS_Inappropriate_Container        |
           ASIS_Inappropriate_Compilation_Unit |
           ASIS_Inappropriate_Element          |
           ASIS_Inappropriate_Line             |
           ASIS_Inappropriate_Line_Number      |
           ASIS_Failed                         =>
         Print_Exception_Info (Ex);
         Put_Line
           ("ASIS Error Status is " &
              Asis.Errors.Error_Kinds'Wide_Image (Asis.Implementation.Status));
         Put_Line
           ("ASIS Diagnosis is " & (Asis.Implementation.Diagnosis));
            Asis.Implementation.Set_Status (Asis.Errors.Not_An_Error);
         Put_Line ("Continuing...");

      when X : others =>
         Print_Exception_Info (X);
         Put_Line ("Continuing...");
   end Process_Units;

end Asis_Tool_2.Context;
