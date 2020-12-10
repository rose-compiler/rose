with Asis.Ada_Environments;
with Asis.Compilation_Units;
with Asis.Exceptions;
with Asis.Errors;
with Asis.Implementation;
with GNAT.Directory_Operations;

with Asis_Tool_2.Unit;
with a_nodes_h.Support;

package body Asis_Tool_2.Context is

   procedure Set_Context
     (Asis_Context : in Asis.Context;
      A_Nodes      : in Standard.A_Nodes.Access_Class)
   is
      use Asis.Ada_Environments;
   begin
      A_Nodes.Set
        (Context =>
           (Name => To_Chars_Ptr (Name (Asis_Context)),
            Parameters => To_Chars_Ptr (Parameters (Asis_Context)),
            Debug_Image => To_Chars_Ptr (Debug_Image (Asis_Context))));
   end Set_Context;

   procedure Process_Units
     (This    : in out Class;
      Options : in     Unit.Options_Record;
      Outputs : in     Outputs_Record)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process_Units";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      use Asis.Exceptions;
      Units : Asis.Compilation_Unit_List :=
        Asis.Compilation_Units.Compilation_Units (This.Asis_Context);
   begin
      for Unit of Units loop
         declare
            Tool_Unit : Asis_Tool_2.Unit.Class;
         begin
            Tool_Unit.Process (Unit, Options, Outputs);
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
         Log ("Caught ASIS exception: " & AEX.Exception_Name (Ex));
         Log ("ASIS Error Status: " &
              Asis.Errors.Error_Kinds'Image (Asis.Implementation.Status));
         Log ("ASIS Diagnosis: " & To_String (Asis.Implementation.Diagnosis));
         Log ("Reraising");
         raise;
   end Process_Units;

   ------------
   -- EXPORTED:
   ------------
   procedure Process
     (This           : in out Class;
      Tree_File_Name : in     String;
      Unit_Options   : in     Unit.Options_Record;
      Outputs        : in     Outputs_Record)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      procedure Begin_Environment is begin
         Asis.Ada_Environments.Associate
           (The_Context => This.Asis_Context,
            Name        => To_Wide_String (Tree_File_Name),
            Parameters  => To_Wide_String ("-C1 " & Tree_File_Name));
         Asis.Ada_Environments.Open (This.Asis_Context);
         Trace_Put_Line ("Context info: " & Asis.Ada_Environments.Debug_Image
                         (This.Asis_Context));
      end;
      procedure End_Environment is begin
         Asis.Ada_Environments.Close (This.Asis_Context);
         Asis.Ada_Environments.Dissociate (This.Asis_Context);
      end;
   begin
      Log ("BEGIN");
      Log ("Tree_File_Name => """ & Tree_File_Name & """");
      Begin_Environment;
      -- Call Begin_Environment first:
      Outputs.Graph.Set_ID
        ("""" & To_String (Asis.Ada_Environments.Name (This.Asis_Context)) & """");
      Set_Context (This.Asis_Context, Outputs.A_Nodes);
      Process_Units (This, Unit_Options, Outputs);
      End_Environment;
      Log ("END");
   end Process;

end Asis_Tool_2.Context;
