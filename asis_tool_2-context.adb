with Ada.Exceptions;
with Ada.Wide_Text_IO;
with Asis;
with Asis.Ada_Environments;
with Asis.Compilation_Units;
with Asis.Exceptions; use ASIS.Exceptions;
with Asis.Errors;
with Asis.Implementation;
with GNAT.Directory_Operations;

with Asis_Tool_2.Unit;

package body Asis_Tool_2.Context is

   ------------
   -- EXPORTED:
   ------------
   procedure Process
     (This : in out Class;
     Graph : in     Dot.Graphs.Access_Class) is
   begin
      This.Graph := Graph;
      -- This just names the Context.  It does not control what it processes:
      Asis.Ada_Environments.Associate
        (This.Asis_Context,
         To_Wide_String (GNAT.Directory_Operations.Get_Current_Dir));
      Asis.Ada_Environments.Open (This.Asis_Context);
      This.Graph.Set_ID
        ("""" & To_String (Asis.Ada_Environments.Name (This.Asis_Context)) & """");
      This.Process_Units;
      Asis.Ada_Environments.Close (This.Asis_Context);
      Asis.Ada_Environments.Dissociate (This.Asis_Context);
   end Process;

   -----------
   -- PRIVATE:
   -----------
   procedure Process_Units (This : in out Class) is
      use Asis.Exceptions;
      Asis_Units : Asis.Compilation_Unit_List :=
        Asis.Compilation_Units.Compilation_Units (This.Asis_Context);
   begin
      for Asis_Unit of Asis_Units loop
         declare
            Tool_Unit : Asis_Tool_2.Unit.Class;
         begin
            Tool_Unit.Process (Asis_Unit, This.Graph);
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
         Awti.Put_Line
           ("ASIS Error Status is " &
              Asis.Errors.Error_Kinds'Wide_Image (Asis.Implementation.Status));
         Awti.Put_Line
           ("ASIS Diagnosis is " & (Asis.Implementation.Diagnosis));
            Asis.Implementation.Set_Status (Asis.Errors.Not_An_Error);
         Awti.Put_Line ("Continuing...");

      when X : others =>
         Print_Exception_Info (X);
         Awti.Put_Line ("Continuing...");
   end Process_Units;

end Asis_Tool_2.Context;
