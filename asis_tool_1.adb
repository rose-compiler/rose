-- Handy references:
-- http://www.sigada.org/WG/asiswg/basics.html
-- http://www.sigada.org/WG/asiswg/ASISWG_Results.html
-- http://docs.adacore.com/asis-docs/html/asis_ug/asis_ug.html
-- http://docs.adacore.com/asis-docs/html/asis_rm/asis_rm.html

with Ada.Characters.Handling;
with Ada.Exceptions;
with Ada.Wide_Text_IO;
with Asis;
with Asis.Ada_Environments;
with Asis.Implementation;
with Asis.Exceptions;
with Asis.Errors;

with Context_Processing;

procedure Asis_Tool_1 is
   My_Context : Asis.Context;
   use Asis.Exceptions;
begin
   Asis.Implementation.Initialize;
   Asis.Ada_Environments.Associate
     (The_Context => My_Context,
      Name        => Asis.Ada_Environments.Default_Name);
   Asis.Ada_Environments.Open (My_Context);

   -- Do actual work:
   Context_Processing.Process_Context
     (The_Context => My_Context,
      Trace       => True);

   Asis.Ada_Environments.Close (My_Context);
   Asis.Ada_Environments.Dissociate (My_Context);
   Asis.Implementation.Finalize;
exception
   when Ex : ASIS_Inappropriate_Context |
        ASIS_Inappropriate_Container |
        ASIS_Inappropriate_Compilation_Unit |
        ASIS_Inappropriate_Element |
        ASIS_Inappropriate_Line |
        ASIS_Inappropriate_Line_Number |
        ASIS_Failed =>
      declare
         use Ada.Characters.Handling;
         use Ada.Exceptions;
         use Ada.Wide_Text_IO;
      begin
         Put ("ASIS exception (");
         Put (To_Wide_String (Exception_Name (Ex)));
         Put (") is raised");
         New_Line;
         Put ("ASIS Error Status is ");
         Put (Asis.Errors.Error_Kinds'Wide_Image (Asis.Implementation.Status));
         New_Line;
         Put ("ASIS Diagnosis is ");
         New_Line;
         Put (Asis.Implementation.Diagnosis);
         New_Line;
         Asis.Implementation.Set_Status (Asis.Errors.Not_An_Error);
      end;

   when Ex : others =>
      declare
         use Ada.Characters.Handling;
         use Ada.Exceptions;
         use Ada.Wide_Text_IO;
      begin
         Put (To_Wide_String (Exception_Name (Ex)));
         Put (" is raised (");
         Put (To_Wide_String (Exception_Information (Ex)));
         Put (")");
         New_Line;
      end;
end Asis_Tool_1;
